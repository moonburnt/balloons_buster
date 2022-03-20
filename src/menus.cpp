#include "menus.hpp"
#include "level.hpp"
#include "shared.hpp"

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
        parent->set_current_scene(new MainMenu(parent));
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

// Main menu logic
void MainMenu::call_exit() {
    parent->active = false;
}

void MainMenu::new_game() {
    parent->set_current_scene(new Level());
}

void MainMenu::load_game() {
    // parent->set_current_scene(
    //     Level::load_save(parent, SettingsManager::manager.savefile.value()));
}

void MainMenu::open_settings() {
    // parent->set_current_scene(new SettingsScreen(parent));
}

MainMenu::MainMenu(SceneManager* p) {
    parent = p;

    // TODO: rework this trash
    buttons.add_button(new TextButton(
        shared::assets.sprites["button_default"],
        shared::assets.sprites["button_hover"],
        shared::assets.sprites["button_pressed"],
        shared::assets.sounds["button_hover"],
        shared::assets.sounds["button_clicked"],
        Rectangle{0, 0, 256, 64},
        "New Game"));
    buttons.add_button(new TextButton(
        shared::assets.sprites["button_default"],
        shared::assets.sprites["button_hover"],
        shared::assets.sprites["button_pressed"],
        shared::assets.sounds["button_hover"],
        shared::assets.sounds["button_clicked"],
        Rectangle{0, 0, 256, 64},
        "Settings"));
    buttons.add_button(new TextButton(
        shared::assets.sprites["button_default"],
        shared::assets.sprites["button_hover"],
        shared::assets.sprites["button_pressed"],
        shared::assets.sounds["button_hover"],
        shared::assets.sounds["button_clicked"],
        Rectangle{0, 0, 256, 64},
        "Exit"));

    float center_x = GetScreenWidth() / 2.0f;
    float center_y = GetScreenHeight() / 2.0f;

    // if (SettingsManager::manager.savefile) {
    //     buttons.add_button(new TextButton(
    //         &shared::assets.sprites["button_default"],
    //         &shared::assets.sprites["button_hover"],
    //         &shared::assets.sprites["button_pressed"],
    //         &shared::assets.sounds["button_hover"],
    //         &shared::assets.sounds["button_clicked"],
    //         Rectangle{0, 0, 256, 64},
    //         "Continue"));

    //     buttons[MM_CONTINUE]->set_pos(Vector2{
    //         center_x - buttons[MM_CONTINUE]->get_rect().width / 2,
    //         center_y - 200});
    // }

    buttons[MM_NEWGAME]->set_pos(
        Vector2{center_x - buttons[MM_NEWGAME]->get_rect().width / 2, center_y - 100});

    buttons[MM_SETTINGS]->set_pos(
        Vector2{center_x - buttons[MM_SETTINGS]->get_rect().width / 2, center_y});

    buttons[MM_EXIT]->set_pos(
        Vector2{center_x - buttons[MM_EXIT]->get_rect().width / 2, center_y + 100});
}

void MainMenu::update(float) {
    // TODO: add keyboard controller, toggle manual update mode on and off,
    // depending on what happend the last - some valid key press or mouse movement
    buttons.update();

    // if (SettingsManager::manager.savefile && buttons[MM_CONTINUE]->is_clicked()) {
    //     load_game();
    //     return;
    // }
    if (buttons[MM_NEWGAME]->is_clicked()) {
        new_game();
        return;
    }

    if (buttons[MM_SETTINGS]->is_clicked()) {
        open_settings();
        return;
    }

    if (buttons[MM_EXIT]->is_clicked()) {
        call_exit();
        return;
    }
}

void MainMenu::draw() {
    buttons.draw();
}
