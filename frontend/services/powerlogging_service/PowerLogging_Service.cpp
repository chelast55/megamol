/*
 * Template_Service.cpp
 *
 * Copyright (C) 2021 by MegaMol Team
 * Alle Rechte vorbehalten.
 */

#include "PowerLogging_Service.hpp"

#include <power_overwhelming/convert_string.h>
#include <power_overwhelming/dump_sensors.h>

#include "mmcore/MegaMolGraph.h"
// local logging wrapper for your convenience until central MegaMol logger established
#include "mmcore/utility/log/Log.h"

#include "FrameStatistics.h"
#include "LuaCallbacksCollection.h"
#include "ModuleGraphSubscription.h"
#include "PerformanceManager.h"


static const std::string service_name = "PowerLogging_Service: ";
static void log(std::string const& text) {
    const std::string msg = service_name + text;
    megamol::core::utility::log::Log::DefaultLog.WriteInfo(msg.c_str());
}

static void log_error(std::string const& text) {
    const std::string msg = service_name + text;
    megamol::core::utility::log::Log::DefaultLog.WriteError(msg.c_str());
}

static void log_warning(std::string const& text) {
    const std::string msg = service_name + text;
    megamol::core::utility::log::Log::DefaultLog.WriteWarn(msg.c_str());
}


namespace megamol::frontend {

bool PowerLogging_Service::init(void* configPtr) {
    if (configPtr == nullptr)
        return false;

    return init(*static_cast<Config*>(configPtr));
}

bool PowerLogging_Service::init(const Config& config) {
#ifdef MEGAMOL_USE_POWER
    powerlog_file_path = config.powerlog_file;
    frames_per_flush = config.frames_per_flush;
    frames_per_request = config.frames_per_request;
    asynchronous_logging = config.asynchronous_logging;
    asynchronous_sampling = config.asynchronous_sampling;
    sample_timeout = config.sample_timeout;
    sample_buffer_size = config.sample_buffer_size;
    sensors = config.sensors;

    if (!config.powerlog_file.empty()) {
        if (!powerlog_file.is_open()) {
            powerlog_file = std::ofstream(powerlog_file_path);

            // csv header
            powerlog_buffer << "Sensor Name" << ',' << "Sample Timestamp (ms)" << ','
                            << "Momentary Power Comsumption (W)" << std::endl;
        }
    }

    if (sensors.nvml) {
        try {
            nvml_sensors.resize(nvml_sensor::for_all(nullptr, 0));
            nvml_sensor::for_all(nvml_sensors.data(), nvml_sensors.size());
        } catch (std::exception& ex) {
            log_warning("Exception while initializing ADL sensors");
            log_warning(ex.what());
        }
    }
    if (sensors.adl) {
        try {
            adl_sensors.resize(adl_sensor::for_all(nullptr, 0));
            adl_sensor::for_all(adl_sensors.data(), adl_sensors.size());
        } catch (std::exception& ex) {
            log_warning("Exception while initializing NVML sensors");
            log_warning(ex.what());
        }
    }
    if (sensors.tinkerforge) {
        try {
            std::vector<tinkerforge_sensor_definition> tinkerforge_sensor_definitions;
            tinkerforge_sensor_definitions.resize(tinkerforge_sensor::get_definitions(nullptr, 0));

            auto count = tinkerforge_sensor::get_definitions(
                tinkerforge_sensor_definitions.data(), tinkerforge_sensor_definitions.size());

            if (count < tinkerforge_sensor_definitions.size()) {
                tinkerforge_sensor_definitions.resize(count);
            }

            for (auto& tinkerforge_sensor_definition : tinkerforge_sensor_definitions) {
                tinkerforge_sensors.push_back(tinkerforge_sensor(tinkerforge_sensor_definition));
                tinkerforge_sensors.back().reset();
                tinkerforge_sensors.back().configure(visus::power_overwhelming::sample_averaging::average_of_4,
                    visus::power_overwhelming::conversion_time::microseconds_588,
                    visus::power_overwhelming::conversion_time::microseconds_588);
            }
        } catch (std::exception& ex) {
            log_warning("Exception while initializing tinkerforge sensors");
            log_warning(ex.what());
        }
    }
    
    if (asynchronous_sampling) {
        bind_sensor(adl_sensors);
        bind_sensor(nvml_sensors);
        bind_sensor(tinkerforge_sensors, tinkerforge_sensor_source::power);
    }

    log("initialized successfully");
#endif

    _requestedResourcesNames = {"RegisterLuaCallbacks", frontend_resources::MegaMolGraph_Req_Name, "RenderNextFrame",
        frontend_resources::MegaMolGraph_SubscriptionRegistry_Req_Name, frontend_resources::FrameStatistics_Req_Name};

    return true;
}

void PowerLogging_Service::close() {
#ifdef MEGAMOL_USE_POWER
    if (powerlog_file.is_open()) {
        // flush rest of log
        powerlog_file << powerlog_buffer.rdbuf();
        powerlog_file.close();
    }

    if (asynchronous_sampling) {
        // flush rest of log and exit logging threads cleanly
        for (auto& sc : sampling_containers)
            sc->time_to_die = true;
        for (auto& lt : logging_threads)
            lt.join();

        unbind_sensor(adl_sensors);
        unbind_sensor(nvml_sensors);
        unbind_sensor(tinkerforge_sensors);
    }
#endif
}

std::vector<FrontendResource>& PowerLogging_Service::getProvidedResources() {
    return _providedResourceReferences;
}

const std::vector<std::string> PowerLogging_Service::getRequestedResourceNames() const {
    return _requestedResourcesNames;
}

void PowerLogging_Service::setRequestedResources(std::vector<FrontendResource> resources) {
    _requestedResourcesReferences = resources;

    auto& megamolgraph_subscription = const_cast<frontend_resources::MegaMolGraph_SubscriptionRegistry&>(
        resources[3].getResource<frontend_resources::MegaMolGraph_SubscriptionRegistry>());

    fill_lua_callbacks();
}

void PowerLogging_Service::updateProvidedResources() {

}

void PowerLogging_Service::digestChangedRequestedResources() {

}

void PowerLogging_Service::resetProvidedResources() {

}

void PowerLogging_Service::preGraphRender() {

}

void PowerLogging_Service::postGraphRender() {
#ifdef MEGAMOL_USE_POWER
    frame_counter++;

    if (!asynchronous_sampling) {
        if (frame_counter % frames_per_request == 0) {
            sample_sensor(adl_sensors);
            sample_sensor(nvml_sensors);
            sample_sensor(tinkerforge_sensors);
        }
    }

    if (frame_counter % frames_per_flush == 0) {
        powerlog_file << powerlog_buffer.rdbuf();
    }

}
#endif

template<typename sensor_type>
void PowerLogging_Service::sample_sensor(std::vector<sensor_type>& sensors) {
    for (auto& sensor : sensors) {
        sample_to_log(sensor.sample());
    }
}
template void PowerLogging_Service::sample_sensor(std::vector<visus::power_overwhelming::adl_sensor>& sensors);
template void PowerLogging_Service::sample_sensor(std::vector<visus::power_overwhelming::nvml_sensor>& sensors);
template void PowerLogging_Service::sample_sensor(std::vector<visus::power_overwhelming::tinkerforge_sensor>& sensors);

template<typename sensor_type, typename... Args>
void PowerLogging_Service::bind_sensor(std::vector<sensor_type>& sensors, Args&&... args) {
    for (auto& sensor : sensors) {
        std::string sensor_name = visus::power_overwhelming::convert_string<char>(sensor.name());
        std::string unique_file_path = std::to_string(std::hash<std::string>{}(sensor_name)) + ".csv";
        sampling_containers.push_back(
            std::make_unique<sampling_container>(sensor_name, unique_file_path, sample_buffer_size));
        write_log_header(sampling_containers.back()->powerlog_file);

        // setup asynchronous sampling
        sensor.sample([](const measurement& sample,
                          void* sc_void_pointer) { store_sample_and_flush_if_necessary(sample, sc_void_pointer); },
            std::forward<Args>(args)..., this->sample_timeout, sampling_containers.back().get());

        // setup asynchronous logging
        if (asynchronous_logging)
            logging_threads.emplace_back(flush_powerlog_buffer, sampling_containers.back().get());
    }
}
template void PowerLogging_Service::bind_sensor(std::vector<visus::power_overwhelming::adl_sensor>& sensors);
template void PowerLogging_Service::bind_sensor(std::vector<visus::power_overwhelming::nvml_sensor>& sensors);
//template void PowerLogging_Service::bind_sensor(std::vector<visus::power_overwhelming::tinkerforge_sensor>& sensors);

template<typename... Args>
void PowerLogging_Service::bind_sensor(
    std::vector<visus::power_overwhelming::tinkerforge_sensor>& sensors, Args&&... args) {
    for (auto& sensor : sensors) {
        std::string sensor_name = visus::power_overwhelming::convert_string<char>(sensor.name());
        std::string unique_file_path = std::to_string(std::hash<std::string>{}(sensor_name)) + ".csv";
        sampling_containers.push_back(
            std::make_unique<sampling_container>(sensor_name, unique_file_path, sample_buffer_size));
        write_log_header(sampling_containers.back()->powerlog_file);

        // setup asynchronous sampling
        sensor.sample([](const measurement& sample,
                          void* sc_void_pointer) { store_sample_and_flush_if_necessary(sample, sc_void_pointer); },
            std::forward<Args>(args)..., this->sample_timeout, sampling_containers.back().get());

        // setup asynchronous logging
        if (asynchronous_logging)
            logging_threads.emplace_back(flush_powerlog_buffer, sampling_containers.back().get());
    }
}

template<typename sensor_type>
void PowerLogging_Service::unbind_sensor(std::vector<sensor_type>& sensors) {
    for (auto& sensor : sensors) {
        sensor.sample(nullptr);
    }
}
template void PowerLogging_Service::unbind_sensor(std::vector<visus::power_overwhelming::adl_sensor>& sensors);
template void PowerLogging_Service::unbind_sensor(std::vector<visus::power_overwhelming::nvml_sensor>& sensors);
template void PowerLogging_Service::unbind_sensor(std::vector<visus::power_overwhelming::tinkerforge_sensor>& sensors);

void PowerLogging_Service::store_sample_and_flush_if_necessary(const measurement& sample, void* sc_void_pointer) {
    auto sc_pointer = static_cast<sampling_container*>(sc_void_pointer);
    sc_pointer->active_sampling_lock.lock();
    sc_pointer->active_sample_buffer.emplace_back(sample.timestamp(), sample.power());

    // swap buffers when program is exited or storage buffer is full
    if (sc_pointer->time_to_die || sc_pointer->active_sample_buffer.size() == sc_pointer->buffer_size) {
        sc_pointer->logging_lock.lock();
        sc_pointer->active_sample_buffer.swap(sc_pointer->logging_sample_buffer);
        sc_pointer->logging_lock.unlock();
        sc_pointer->signal.notify_one();
    }
    sc_pointer->active_sampling_lock.unlock();
}

void PowerLogging_Service::flush_powerlog_buffer(sampling_container* sc_pointer) {
    while (true) {
        sc_pointer->logging_lock.lock();
        sc_pointer->signal.wait(sc_pointer->logging_lock);
        for (compact_sample& sample : sc_pointer->logging_sample_buffer) {
            sample_to_log(sc_pointer->powerlog_file, sc_pointer->name, sample);
        }
        sc_pointer->logging_sample_buffer.clear();
        sc_pointer->logging_lock.unlock();

        if (sc_pointer->time_to_die)
            break;
    }
}

void PowerLogging_Service::write_log_header(std::ofstream& log_file) {
    log_file << "Sensor Name" << ',' << "Sample Timestamp (ms)" << ',' << "Momentary Power Comsumption (W)"
                    << std::endl;
}

void PowerLogging_Service::sample_to_log(std::ofstream& log_file, const std::string& name, const compact_sample& sample) {
    log_file << name << ',' << sample.timestamp << ',' << sample.value << std::endl;
}

void PowerLogging_Service::sample_to_log(const measurement& sample) {
    powerlog_buffer << visus::power_overwhelming::convert_string<char>(sample.sensor()) << ',' << sample.timestamp()
                    << ',' << sample.power() << std::endl;
}

void PowerLogging_Service::fill_lua_callbacks() {
    frontend_resources::LuaCallbacksCollection callbacks;

    auto& graph =
        const_cast<core::MegaMolGraph&>(_requestedResourcesReferences[1].getResource<core::MegaMolGraph>());
    auto& render_next_frame = _requestedResourcesReferences[2].getResource<std::function<bool()>>();

    callbacks.add<frontend_resources::LuaCallbacksCollection::VoidResult>(
        "mmFlushPowerlog", "()", {[&]() -> frontend_resources::LuaCallbacksCollection::VoidResult {
            this->onLuaFlushPowerlog();
            return frontend_resources::LuaCallbacksCollection::VoidResult{};
        }});

    callbacks.add<frontend_resources::LuaCallbacksCollection::VoidResult>(
        "mmSwapPowerlogBuffers", "()", {[&]() -> frontend_resources::LuaCallbacksCollection::VoidResult {
            this->onLuaSwapPowerlogBuffers(false, false);
            return frontend_resources::LuaCallbacksCollection::VoidResult{};
        }});

    callbacks.add<frontend_resources::LuaCallbacksCollection::VoidResult, bool, bool>("mmSwapAndClearPowerlogBuffers",
        "(bool clear_active_buffer, bool clear_logging_buffer)",
        {[&](bool clear_active_buffer,
             bool clear_logging_buffer) -> frontend_resources::LuaCallbacksCollection::VoidResult {
            this->onLuaSwapPowerlogBuffers(clear_active_buffer, clear_logging_buffer);
            return frontend_resources::LuaCallbacksCollection::VoidResult{};
        }});

    auto& register_callbacks =
        _requestedResourcesReferences[0]
            .getResource<std::function<void(frontend_resources::LuaCallbacksCollection const&)>>();

    register_callbacks(callbacks);
}

void PowerLogging_Service::onLuaFlushPowerlog() {
    for (auto& sc : sampling_containers) {
        sc.get()->time_to_die = true; // convenient way for waiting for current sample to finish, swap buffers
        flush_powerlog_buffer(sc.get()); // loop only executed once because of above line
        sc.get()->time_to_die = false;
        log("LUA: flushed powerlog!");
    }
}

void PowerLogging_Service::onLuaSwapPowerlogBuffers(bool clear_active_buffer, bool clear_logging_buffer) {
    for (auto& sc : sampling_containers) {
        sc.get()->active_sampling_lock.lock();
        sc.get()->logging_lock.lock();
        if (clear_active_buffer) {
            sc.get()->active_sample_buffer.clear();
            log("LUA: cleared active sample buffer!");
        }
        if (clear_logging_buffer) {
            sc.get()->logging_sample_buffer.clear();
            log("LUA: cleared logging sample buffer!");
        }
        sc.get()->active_sample_buffer.swap(sc.get()->logging_sample_buffer);
        log("LUA: swapped powerlog buffers!");
        sc.get()->active_sampling_lock.unlock();
        sc.get()->logging_lock.unlock();
    }
}

std::stringstream PowerLogging_Service::powerlog_buffer;


} // namespace megamol::frontend
