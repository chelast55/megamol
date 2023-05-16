/*
 * Template_Service.cpp
 *
 * Copyright (C) 2021 by MegaMol Team
 * Alle Rechte vorbehalten.
 */

#include "PowerLogging_Service.hpp"

// local logging wrapper for your convenience until central MegaMol logger established
#include "mmcore/utility/log/Log.h"

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
    asynchronous_logging = config.asynchronous_logging;
    asynchronous_sampling = config.asynchronous_sampling;
    sensors = config.sensors;
    if (!config.powerlog_file.empty()) {
        if (!powerlog_file.is_open()) {
            powerlog_file = std::ofstream(powerlog_file_path);

            // csv header
            powerlog_buffer << "Sensor Type" << ',' << "Sensor Name" << ',' << "Sample Timestamp (ms)" << ','
                       << "Momentary Power Comsumption (W)" << std::endl;
        }
    }
    frames_per_request = config.frames_per_request;

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

    unbind_sensor(adl_sensors);
    unbind_sensor(nvml_sensors);
    unbind_sensor(tinkerforge_sensors);
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

    if (frame_counter % frames_per_flush == 0)
        powerlog_file << powerlog_buffer.rdbuf();
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
        sensor.sample([](const measurement& sample, void*) { PowerLogging_Service::sample_to_log(sample); }, this->request_timeout, this);
    }
}
template void PowerLogging_Service::bind_sensor(std::vector<visus::power_overwhelming::adl_sensor>& sensors);
template void PowerLogging_Service::bind_sensor(std::vector<visus::power_overwhelming::nvml_sensor>& sensors);

void PowerLogging_Service::bind_sensor(std::vector<visus::power_overwhelming::tinkerforge_sensor>& sensors) {
    for (auto& sensor : sensors) {
        sensor.sample([](const measurement& sample, void*) { PowerLogging_Service::sample_to_log(sample); },
            tinkerforge_sensor_source::power, this->request_timeout, this);
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
template void PowerLogging_Service::unbind_sensor(
    std::vector<visus::power_overwhelming::tinkerforge_sensor>& sensors);

void PowerLogging_Service::sample_to_log(const measurement& sample) {
    powerlog_buffer << ""/*TODO: somehow get sensor type*/ << ',' << sample.sensor() << ',' << sample.timestamp()
                    << ',' << sample.power() << std::endl;
}

std::stringstream PowerLogging_Service::powerlog_buffer;


} // namespace megamol::frontend
