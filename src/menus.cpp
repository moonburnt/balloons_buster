#include "menus.hpp"
#include "common.hpp"
#include "engine/ui.hpp"
#include "level.hpp"
#include "shared.hpp"
#include "spdlog/spdlog.h"
#include "tomlplusplus/toml.hpp"
#include <raylib.h>

// Title Screen
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

// Settings Screen
class SettingsScreen : public Scene {
private:
    SceneManager* parent;

    // It may be done without this thing, but will do for now
    toml::table current_settings;

    Label title;
    Label unsaved_changes_msg;
    bool settings_changed;

    TextButton* save_button;
    Button* exit_button;

    Label show_fps_title;
    Checkbox* fps_cb;

    Label fullscreen_title;
    Checkbox* fullscreen_cb;

    void exit_to_menu() {
        spdlog::info("Switching to main menu");
        exit_button->reset_state();
        parent->set_current_scene(new MainMenu(parent));
    }

    void save_settings() {
        save_button->reset_state();
        if (!settings_changed) return;

        shared::config.settings = current_settings;
        shared::config.save();

        fps_cb->reset_state();
        fullscreen_cb->reset_state();

        settings_changed = false;

        if (shared::config.settings["show_fps"].value_exact<bool>().value()) {
            if (shared::window.sc_mgr.nodes.count("fps_counter") == 0) {
                shared::window.sc_mgr.nodes["fps_counter"] = new FrameCounter();
            }
        }
        else {
            shared::window.sc_mgr.nodes.erase("fps_counter");
        }

        if (shared::config.settings["fullscreen"].value_or(false)) {
            if (!IsWindowFullscreen()) {
                const int current_screen = GetCurrentMonitor();
                ToggleFullscreen();
                SetWindowSize(
                    GetMonitorWidth(current_screen),
                    GetMonitorHeight(current_screen));
            };
        }
        else {
            if (IsWindowFullscreen()) {
                SetWindowSize(
                    shared::config.settings["resolution"][0].value_or(1280),
                    shared::config.settings["resolution"][1].value_or(720));
                ToggleFullscreen();
            };
        }
    }

public:
    SettingsScreen(SceneManager* p)
        : parent(p)
        , current_settings(shared::config.settings) // this should get copied
        , title("Settings", GetScreenWidth() / 2, 30)
        , unsaved_changes_msg(
              "Settings changed. Press save to apply!", GetScreenWidth() / 2, 60)
        , settings_changed(false)
        , save_button(make_text_button("Save"))
        , exit_button(make_close_button())
        , show_fps_title("Show FPS:", {30.0f, 100.0f})
        , fps_cb(make_checkbox(
              shared::config.settings["show_fps"].value_exact<bool>().value()))
        , fullscreen_title("Fullscreen:", {30.0f, 150.0f})
        , fullscreen_cb(make_checkbox(
              shared::config.settings["fullscreen"].value_exact<bool>().value())) {
        title.center();
        unsaved_changes_msg.center();

        save_button->set_pos(
            {GetScreenWidth() / 2.0f - save_button->get_rect().width / 2.0f,
             GetScreenHeight() - 100.0f});

        exit_button->set_pos({static_cast<float>(GetScreenWidth() - (30 + 64)), 30.0f});
        fps_cb->set_pos({200.0f, 100.0f});
        fullscreen_cb->set_pos({200.0f, 150.0f});
    }

    ~SettingsScreen() {
        delete fps_cb;
        delete fullscreen_cb;
        delete exit_button;
    }

    void update(float) override {
        save_button->update();
        exit_button->update();
        fps_cb->update();
        fullscreen_cb->update();

        if (exit_button->is_clicked()) {
            exit_to_menu();
            return;
        }

        if (save_button->is_clicked()) {
            save_settings();
            return;
        }

        if (fps_cb->is_clicked()) {
            current_settings.insert_or_assign("show_fps", fps_cb->get_toggle());
            settings_changed = true;
        }
        else if (fullscreen_cb->is_clicked()) {
            current_settings.insert_or_assign("fullscreen", fullscreen_cb->get_toggle());
            settings_changed = true;
        }
        else settings_changed = false;
    }

    void draw() override {
        title.draw();

        show_fps_title.draw();
        fullscreen_title.draw();

        save_button->draw();
        exit_button->draw();
        fps_cb->draw();
        fullscreen_cb->draw();

        if (settings_changed) {
            unsaved_changes_msg.draw();
        }
    }
};

// Main menu
void MainMenu::call_exit() {
    parent->active = false;
}

void MainMenu::new_game() {
    spdlog::info("Switching to level");
    parent->set_current_scene(new Level());
}

void MainMenu::load_game() {
    // parent->set_current_scene(
    //     Level::load_save(parent, SettingsManager::manager.savefile.value()));
}

void MainMenu::open_settings() {
    spdlog::info("Switching to settings");
    parent->set_current_scene(new SettingsScreen(parent));
}

MainMenu::MainMenu(SceneManager* p) {
    parent = p;

    buttons.add_button(make_text_button("New Game"));
    buttons.add_button(make_text_button("Settings"));
    buttons.add_button(make_text_button("Exit"));

    float center_x = GetScreenWidth() / 2.0f;
    float center_y = GetScreenHeight() / 2.0f;

    // if (SettingsManager::manager.savefile) {
    //     buttons.add_button(make_text_button("Continue"));

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
