#include "engine/utility.hpp"
#include "menus.hpp"
#include "shared.hpp"
#include <algorithm>
#include <iostream>

#define ASSET_PATH "./Assets/"
static constexpr const char* SPRITE_PATH = ASSET_PATH "Sprites/";
static constexpr const char* SFX_PATH = ASSET_PATH "SFX/";

static constexpr const char* SPRITE_FORMAT = ".png";
static constexpr const char* SFX_FORMAT = ".ogg";

int main() {
    // Window's instance is initialized in shared.cpp
    // window.init() will configure it afterwards
    shared::config.load();

    // TODO: maybe move this check somewhere else?
    // Originally I've intended to clamp it, but its impossible to do before
    // screen has been initialized. So we can only ensure that our res is higher
    // than lowest possible resolution.
    shared::window.init(
        std::max(shared::config.settings["resolution"][0].value_or(1280), 1280),
        std::max(shared::config.settings["resolution"][1].value_or(720), 720),
        "Balloon Buster");

    shared::assets.sprites.load(SPRITE_PATH, SPRITE_FORMAT);
    shared::assets.sounds.load(SFX_PATH, SFX_FORMAT);

    if (shared::config.settings["show_fps"].value_or(false)) {
        shared::window.sc_mgr.nodes["fps_counter"] = new FrameCounter();
    };

    shared::window.sc_mgr.set_current_scene(new TitleScreen(&shared::window.sc_mgr));
    shared::window.run();

    return 0;
}
