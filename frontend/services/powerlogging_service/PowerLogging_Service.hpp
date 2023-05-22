/*
 * Template_Service.hpp
 *
 * Copyright (C) 2021 by MegaMol Team
 * Alle Rechte vorbehalten.
 */

#pragma once

#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>

#include <power_overwhelming/adl_sensor.h>
#include <power_overwhelming/nvml_sensor.h>
#include <power_overwhelming/tinkerforge_sensor_definiton.h> // yes, it doesn't work without the typo in "definiton"
#include <power_overwhelming/tinkerforge_sensor_source.h>
#include <power_overwhelming/tinkerforge_display.h>

#include "AbstractFrontendService.hpp"

namespace megamol::frontend {

class PowerLogging_Service final : public AbstractFrontendService {

public:
    using sensor = visus::power_overwhelming::sensor;
    using adl_sensor = visus::power_overwhelming::adl_sensor;
    using nvml_sensor = visus::power_overwhelming::nvml_sensor;
    using tinkerforge_sensor_definition = visus::power_overwhelming::tinkerforge_sensor_definiton; // typo in "definiton". This is fine ...for now
    using tinkerforge_sensor_source = visus::power_overwhelming::tinkerforge_sensor_source;
    using tinkerforge_sensor = visus::power_overwhelming::tinkerforge_sensor;
    using measurement = visus::power_overwhelming::measurement;
    using measurement_callback = visus::power_overwhelming::measurement_callback;

    struct Config {
        struct Sensors {
            bool adl;
            bool nvml;
            bool tinkerforge;
            bool rapl;
        };

        std::string powerlog_file;
        uint32_t frames_per_flush;
        uint32_t frames_per_request;
        sensor::microseconds_type request_timeout;
        bool asynchronous_logging;
        bool asynchronous_sampling;
        Sensors sensors;
    };

    std::string serviceName() const override {
        return "PowerLogging_Service";
    }

    bool init(const Config& config);
    bool init(void* configPtr) override;
    void close() override;


    std::vector<FrontendResource>& getProvidedResources() override;
    const std::vector<std::string> getRequestedResourceNames() const override;

    void setRequestedResources(std::vector<FrontendResource> resources) override;

    void updateProvidedResources() override;

    void digestChangedRequestedResources() override;

    void resetProvidedResources() override;

    void preGraphRender() override;
    void postGraphRender() override;

private:

    struct compact_sample {
        measurement::timestamp_type timestamp;
        measurement::value_type value;

        compact_sample(measurement::timestamp_type ts, measurement::value_type v) : timestamp(ts), value(v) {}
    };

    struct sampling_container {
        std::string name;
        std::mutex lock;
        std::vector<compact_sample> sample_buffer;

        sampling_container(const sampling_container& c) : name(c.name), lock(), sample_buffer(c.sample_buffer) {}
        sampling_container(std::string n, size_t buffer_size) : name(n), lock(), sample_buffer() {
            sample_buffer.reserve(buffer_size);
        }
    };

    // resources
    std::vector<FrontendResource> _providedResourceReferences;
    std::vector<std::string> _requestedResourcesNames;
    std::vector<FrontendResource> _requestedResourcesReferences;

    // config parameters
    std::string powerlog_file_path;
    std::ofstream powerlog_file;
    uint32_t frames_per_flush = 1;
    uint32_t frames_per_request = 1;
    sensor::microseconds_type request_timeout = 5000;
    bool asynchronous_logging = true;
    bool asynchronous_sampling = true;
    Config::Sensors sensors = {false, false, false, false};

    // sensors
    std::vector<adl_sensor> adl_sensors;
    std::vector<nvml_sensor> nvml_sensors;
    std::vector<tinkerforge_sensor> tinkerforge_sensors;

    // other helper variables
    static std::stringstream powerlog_buffer;
    uint64_t frame_counter = 0;

    // sampling
    std::vector<sampling_container> sampling_containers;

    template<typename T>
    void sample_sensor(std::vector<T>& sensors);
    template<typename T>
    void bind_sensor(std::vector<T>& sensors);
    void bind_sensor(std::vector<tinkerforge_sensor>& sensors);
    template<typename T>
    void unbind_sensor(std::vector<T>& sensors);

    // helper functions
    static void sample_to_log(const measurement& sample);
};

} // namespace megamol::frontend
