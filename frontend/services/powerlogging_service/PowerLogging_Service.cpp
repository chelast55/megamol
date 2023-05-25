/*
 * Template_Service.cpp
 *
 * Copyright (C) 2021 by MegaMol Team
 * Alle Rechte vorbehalten.
 */

#include "PowerLogging_Service.hpp"

#include <power_overwhelming/convert_string.h>


// local logging wrapper for your convenience until central MegaMol logger established
#include "mmcore/utility/log/Log.h"

#include "power_overwhelming/dump_sensors.h"

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
            }
        } catch (std::exception& ex) {
            log_warning("Exception while initializing tinkerforge sensors");
            log_warning(ex.what());
        }
    }
    
    if (asynchronous_sampling) {
        bind_sensor(adl_sensors);
        bind_sensor(nvml_sensors);
        bind_sensor(tinkerforge_sensors);
    }

    log("initialized successfully");
#endif
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
            sc.time_to_die = true;
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

template<typename T>
void PowerLogging_Service::sample_sensor(std::vector<T>& sensors)
{
    for (auto& sensor : sensors) {
        sample_to_log(sensor.sample());
    }
}
template void PowerLogging_Service::sample_sensor(std::vector<visus::power_overwhelming::adl_sensor>& sensors);
template void PowerLogging_Service::sample_sensor(std::vector<visus::power_overwhelming::nvml_sensor>& sensors);
template void PowerLogging_Service::sample_sensor(std::vector<visus::power_overwhelming::tinkerforge_sensor>& sensors);

template<typename T>
void PowerLogging_Service::bind_sensor(std::vector<T>& sensors) {
    for (auto& sensor : sensors) {
        std::string sensor_name = visus::power_overwhelming::convert_string<char>(sensor.name());
        std::string unique_file_path = std::to_string(std::hash<std::string>{}(sensor_name)) + ".csv";
        sampling_containers.emplace_back(sensor_name, unique_file_path, sample_buffer_size);
        write_log_header(sampling_containers.back().powerlog_file);

        // setup asynchronous sampling
        sensor.sample(
            [](const measurement& sample, void* sc_void_pointer) {
                auto sc_pointer = static_cast<sampling_container*>(sc_void_pointer);
                sc_pointer->storage_lock.lock();
                sc_pointer->storage_sample_buffer.emplace_back(sample.timestamp(), sample.power());

                // swap buffers when program is exited or storage buffer is full
                if (sc_pointer->time_to_die || sc_pointer->storage_sample_buffer.size() ==
                    sc_pointer->buffer_size) {
                    sc_pointer->logging_lock.lock();
                    sc_pointer->storage_sample_buffer.swap(
                        sc_pointer->logging_sample_buffer);
                    sc_pointer->logging_lock.unlock();
                    sc_pointer->signal.notify_one();
                }
                sc_pointer->storage_lock.unlock();
            },
            this->sample_timeout, &sampling_containers.back());

        // setup asynchronous logging
        logging_threads.emplace_back(
            [](sampling_container* sc_pointer) {
                while (true) {
                    sc_pointer->logging_lock.lock();
                    sc_pointer->signal.wait(sc_pointer->logging_lock);
                    for (compact_sample& sample : sc_pointer->logging_sample_buffer) {
                        sample_to_log(
                            sc_pointer->powerlog_file, sc_pointer->name, sample);
                    }
                    sc_pointer->logging_sample_buffer.clear();
                    sc_pointer->logging_lock.unlock();

                    if (sc_pointer->time_to_die)
                        break;
                }
            },
            &sampling_containers.back());
    }
}
template void PowerLogging_Service::bind_sensor(std::vector<visus::power_overwhelming::adl_sensor>& sensors);
template void PowerLogging_Service::bind_sensor(std::vector<visus::power_overwhelming::nvml_sensor>& sensors);

void PowerLogging_Service::bind_sensor(std::vector<visus::power_overwhelming::tinkerforge_sensor>& sensors) {
    for (auto& sensor : sensors) {
        std::string sensor_name = visus::power_overwhelming::convert_string<char>(sensor.name());
        std::string unique_file_path = std::to_string(std::hash<std::string>{}(sensor_name)) + ".csv";
        sampling_containers.emplace_back(sensor_name, unique_file_path, sample_buffer_size);
        write_log_header(sampling_containers.back().powerlog_file);

        // setup asynchronous sampling
        sensor.sample(
            [](const measurement& sample, void* sc_void_pointer) {
                auto sc_pointer = static_cast<sampling_container*>(sc_void_pointer);
                sc_pointer->storage_lock.lock();
                sc_pointer->storage_sample_buffer.emplace_back(sample.timestamp(), sample.power());

                // swap buffers when program is exited or storage buffer is full
                if (sc_pointer->time_to_die || sc_pointer->storage_sample_buffer.size() == sc_pointer->buffer_size) {
                    sc_pointer->logging_lock.lock();
                    sc_pointer->storage_sample_buffer.swap(sc_pointer->logging_sample_buffer);
                    sc_pointer->logging_lock.unlock();
                    sc_pointer->signal.notify_one();
                }
                sc_pointer->storage_lock.unlock();
            },
            tinkerforge_sensor_source::power, this->sample_timeout, &sampling_containers.back());

        // setup asynchronous logging
        logging_threads.emplace_back(
            [](sampling_container* sc_pointer) {
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
            },
            &sampling_containers.back());
    }
}

template<typename T>
void PowerLogging_Service::unbind_sensor(std::vector<T>& sensors) {
    for (auto& sensor : sensors) {
        sensor.sample(nullptr);
    }
}
template void PowerLogging_Service::unbind_sensor(std::vector<visus::power_overwhelming::adl_sensor>& sensors);
template void PowerLogging_Service::unbind_sensor(std::vector<visus::power_overwhelming::nvml_sensor>& sensors);
template void PowerLogging_Service::unbind_sensor(std::vector<visus::power_overwhelming::tinkerforge_sensor>& sensors);

void PowerLogging_Service::sample_to_log(const measurement& sample) {
    powerlog_buffer << visus::power_overwhelming::convert_string<char>(sample.sensor()) << ',' << sample.timestamp()
                    << ',' << sample.power() << std::endl;
}

void PowerLogging_Service::write_log_header(std::ofstream& log_file) {
    log_file << "Sensor Name" << ',' << "Sample Timestamp (ms)" << ',' << "Momentary Power Comsumption (W)"
                    << std::endl;
}

void PowerLogging_Service::sample_to_log(std::ofstream& log_file, const std::string& name, const compact_sample& sample) {
    log_file << name << ',' << sample.timestamp << ',' << sample.value << std::endl;
}

std::stringstream PowerLogging_Service::powerlog_buffer;


} // namespace megamol::frontend
