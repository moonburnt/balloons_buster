#include "shared.hpp"

static constexpr const char* SETTINGS_PATH = "./settings.toml";

GameWindow shared::window;
AssetLoader shared::assets;
SettingsManager shared::config = SettingsManager(SETTINGS_PATH);
