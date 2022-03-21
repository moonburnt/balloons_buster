#include "engine/utility.hpp"
#include "menus.hpp"
#include "shared.hpp"

#define ASSET_PATH "./Assets/"
static constexpr const char* SPRITE_PATH = ASSET_PATH "Sprites/";
static constexpr const char* SFX_PATH = ASSET_PATH "SFX/";

static constexpr const char* SPRITE_FORMAT = ".png";
static constexpr const char* SFX_FORMAT = ".ogg";

int main() {
    // Window's instance is initialized in shared.cpp
    // window.init() will configure it afterwards
    shared::window.init(1280, 720, "Balloon Buster");

    shared::assets.sprites.load(SPRITE_PATH, SPRITE_FORMAT);
    shared::assets.sounds.load(SFX_PATH, SFX_FORMAT);

    shared::window.sc_mgr.add_node(new FrameCounter());

    shared::window.sc_mgr.set_current_scene(new TitleScreen(&shared::window.sc_mgr));
    shared::window.run();

    return 0;
}
