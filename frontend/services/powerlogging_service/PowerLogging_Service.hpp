/*
 * Template_Service.hpp
 *
 * Copyright (C) 2021 by MegaMol Team
 * Alle Rechte vorbehalten.
 */

#pragma once

#include <fstream>
#include <sstream>

#include "AbstractFrontendService.hpp"

#include "power_overwhelming/power_overwhelming_api.h"
#include "power_overwhelming/adl_sensor.h"
#include "power_overwhelming/nvml_sensor.h"
#include "power_overwhelming/tinkerforge_sensor_definiton.h" // yes, it doesn't work without the typo in "definiton"
#include "power_overwhelming/tinkerforge_display.h"

namespace megamol::frontend {

class PowerLogging_Service final : public AbstractFrontendService {
public:
    struct Config {
        struct Sensors {
            bool adl;
            bool nvml;
            bool tinkerforge;
            bool rapl;
        };

        std::string log_file;
        uint32_t frames_per_flush;
        uint32_t frames_per_request;
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

    // resources
    std::vector<FrontendResource> _providedResourceReferences;
    std::vector<std::string> _requestedResourcesNames;
    std::vector<FrontendResource> _requestedResourcesReferences;

    // config parameters
    std::ofstream log_file;
    uint32_t frames_per_flush = 1;
    uint32_t frames_per_request = 1;
    bool asynchronous_logging = true;
    bool asynchronous_sampling = true;
    Config::Sensors sensors = {false, false, false, false};

    // sensors
    std::vector<visus::power_overwhelming::adl_sensor> adl_sensors;
    std::vector<visus::power_overwhelming::nvml_sensor> nvml_sensors;
    std::vector<visus::power_overwhelming::tinkerforge_sensor> tinkerforge_sensors;

    // other helper variables
    std::stringstream log_buffer;
    uint64_t frame_counter = 0;

    // helper functions
    template<typename T>
    void sample_sensor(const std::string& sensor_type, std::vector<T>& sensors);
};

} // namespace megamol::frontend
