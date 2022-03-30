#include "shared.hpp"
#include "tomlplusplus/toml.hpp"

static constexpr const char* SETTINGS_PATH = "./settings.toml";

GameWindow shared::window;
AssetLoader shared::assets;
SettingsManager shared::config = SettingsManager(
    toml::table{
        {"show_fps", true},
        {"fullscreen", false},
        {"resolution", toml::array{1280, 720}},
        {"sfx_volume", 100},
        {"music_volume", 100}},
    SETTINGS_PATH);
