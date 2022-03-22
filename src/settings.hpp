#pragma once

#include "tomlplusplus/toml.hpp"
#include <string>

class SettingsManager {
private:
    std::string settings_path;

    toml::table default_settings;

public:
    SettingsManager(std::string settings_path);

    toml::table settings;

    // Save settings to file on provided path
    void save();
    // Load settings from provided path
    bool load();
    // Reset settings to defaults
    void reset();
};
