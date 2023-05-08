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
    frames_per_flush = config.frames_per_flush;
    asynchronous_logging = config.asynchronous_logging;
    asynchronous_sampling = config.asynchronous_sampling;
    sensors = config.sensors;
    if (!config.log_file.empty()) {
        if (!log_file.is_open()) {
            log_file = std::ofstream(config.log_file);

            // csv header
            log_buffer << "Sensor Type" << ',' << "Sensor Name" << ',' << "Sample Timestamp (ms)" << ','
                       << "Momentary Power Comsumption (W)" << std::endl;
        }
    }
    frames_per_request = config.frames_per_request;

    if (sensors.nvml) {
        try {
            using nvml_sensor = visus::power_overwhelming::nvml_sensor;
            nvml_sensors.resize(nvml_sensor::for_all(nullptr, 0));
            nvml_sensor::for_all(nvml_sensors.data(), nvml_sensors.size());
        }
        catch (std::exception& ex) {
            log_warning("Exception while initializing ADL sensors");
            log_warning(ex.what());
        }
    }
    if (sensors.adl) {
        try {
            using adl_sensor = visus::power_overwhelming::adl_sensor;
            adl_sensors.resize(adl_sensor::for_all(nullptr, 0));
            adl_sensor::for_all(adl_sensors.data(), adl_sensors.size());
        }
        catch (std::exception& ex) {
            log_warning("Exception while initializing NVML sensors");
            log_warning(ex.what());
        }
    }
    if (sensors.tinkerforge) {
        try {
            using tinkerforge_sensor_definition = visus::power_overwhelming::tinkerforge_sensor_definiton; // typo in "definiton". This is fine ...for now
            using tinkerforge_sensor = visus::power_overwhelming::tinkerforge_sensor;
            std::vector<tinkerforge_sensor_definition> tinkerforge_sensor_definitions;
            tinkerforge_sensor_definitions.resize(tinkerforge_sensor::get_definitions(nullptr, 0));

            auto count = tinkerforge_sensor::get_definitions(
                tinkerforge_sensor_definitions.data(), tinkerforge_sensor_definitions.size());

            if (count < tinkerforge_sensor_definitions.size()) {
                tinkerforge_sensor_definitions.resize(count);
            }

            // definitions to sensors (Is this allowed? Is this a bad idea?)
            for (auto& tinkerforge_sensor_definition : tinkerforge_sensor_definitions) {
                tinkerforge_sensors.push_back(tinkerforge_sensor(tinkerforge_sensor_definition));
            }
        }
        catch (std::exception& ex) {
            log_warning("Exception while initializing tinkerforge sensors");
            log_warning(ex.what());
        }
    }

    log("initialized successfully");
#endif
    return true;
}

void PowerLogging_Service::close() {
#ifdef MEGAMOL_USE_POWER
    if (log_file.is_open()) {
        // flush rest of log
        log_file << log_buffer.rdbuf();
        log_file.close();
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
    frame_counter++;

    if (frame_counter % frames_per_request == 0) {
        sample_sensor("ADL", adl_sensors);
        sample_sensor("NVML", nvml_sensors);
        sample_sensor("Tinkerforge", tinkerforge_sensors);
    }

    if (frame_counter % frames_per_flush == 0)
        log_file << log_buffer.rdbuf();
}

template<typename T>
void PowerLogging_Service::sample_sensor(const std::string& sensor_type, std::vector<T>& sensors)
{
    for (auto& sensor : sensors) {
        auto sample = sensor.sample();
        log_buffer << sensor_type << ',' << sensor.name() << ',' << sample.timestamp() << ',' << sample.power()
                   << std::endl;
    }
}

template void PowerLogging_Service::sample_sensor(
    const std::string& sensor_type, std::vector<visus::power_overwhelming::adl_sensor>& sensors);
template void PowerLogging_Service::sample_sensor(
    const std::string& sensor_type, std::vector<visus::power_overwhelming::nvml_sensor>& sensors);
template void PowerLogging_Service::sample_sensor(
    const std::string& sensor_type, std::vector<visus::power_overwhelming::tinkerforge_sensor>& sensors);


} // namespace megamol::frontend
