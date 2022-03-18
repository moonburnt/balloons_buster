#include "menus.hpp"
#include "level.hpp"

TitleScreen::TitleScreen(SceneManager* p) {
    parent = p;

    greeter_msg = "This game has been made with raylib\0";
    greeter_pos = center_text(
        greeter_msg,
        Vector2{GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f});

    timer = new Timer(2.0f);
    timer->start();
}

void TitleScreen::update(float dt) {
    // TODO: add timer that will automatically switch to main menu after
    // some time has been passed since title's update
    if (timer->tick(dt)) {
        parent->set_current_scene(new Level());
    }
}

void TitleScreen::draw() {
    DrawText(
        greeter_msg.c_str(),
        greeter_pos.x,
        greeter_pos.y,
        DEFAULT_TEXT_SIZE,
        DEFAULT_TEXT_COLOR);
}
