#include "menus.hpp"
#include "common.hpp"
#include "level.hpp"
#include "shared.hpp"
#include "tomlplusplus/toml.hpp"

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

    std::string title_msg;
    Vector2 title_pos;

    std::string unsaved_changes_msg;
    Vector2 unsaved_changes_pos;
    bool settings_changed;

    TextButton* save_button;
    Button* exit_button;

    std::string show_fps_title;
    Vector2 show_fps_pos;
    Checkbox* fps_cb;

    void exit_to_menu() {
        exit_button->reset_state();
        parent->set_current_scene(new MainMenu(parent));
    }

    void save_settings() {
        save_button->reset_state();
        if (!settings_changed) return;

        shared::config.settings = current_settings;
        shared::config.save();

        fps_cb->reset_state();

        settings_changed = false;

        if (shared::config.settings["show_fps"].value_exact<bool>().value()) {
            if (shared::window.sc_mgr.nodes.count("fps_counter") == 0) {
                shared::window.sc_mgr.nodes["fps_counter"] = new FrameCounter();
            }
        }
        else {
            shared::window.sc_mgr.nodes.erase("fps_counter");
        }
    }

public:
    SettingsScreen(SceneManager* p)
        : current_settings(shared::config.settings) // this should get copied
        , save_button(make_text_button("Save"))
        , exit_button(make_close_button())
        , fps_cb(make_checkbox(
              shared::config.settings["show_fps"].value_exact<bool>().value())) {
        parent = p;
        title_msg = "Settings";
        int center_x = GetScreenWidth() / 2;
        title_pos.x = center_text_h(title_msg, center_x);
        title_pos.y = 30;

        unsaved_changes_msg = "Settings changed. Press save to apply!";
        unsaved_changes_pos.x = center_text_h(unsaved_changes_msg, center_x);
        unsaved_changes_pos.y = 60;

        settings_changed = false;

        save_button->set_pos(
            {center_x - save_button->get_rect().width / 2, GetScreenHeight() - 100.0f});

        exit_button->set_pos(
            Vector2{static_cast<float>(GetScreenWidth() - (30 + 64)), 30.0f});

        show_fps_title = "Show FPS:";
        show_fps_pos = Vector2{30.0f, 100.0f};
        fps_cb->set_pos({200.0f, 100.0f});
    }

    ~SettingsScreen() {
        delete fps_cb;
        delete exit_button;
    }

    void update(float) override {
        save_button->update();
        exit_button->update();
        fps_cb->update();

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
        else settings_changed = false;
    }

    void draw() override {
        DrawText(
            title_msg.c_str(),
            title_pos.x,
            title_pos.y,
            DEFAULT_TEXT_SIZE,
            DEFAULT_TEXT_COLOR);

        DrawText(
            show_fps_title.c_str(),
            show_fps_pos.x,
            show_fps_pos.y,
            DEFAULT_TEXT_SIZE,
            DEFAULT_TEXT_COLOR);

        save_button->draw();
        exit_button->draw();
        fps_cb->draw();

        if (settings_changed) {
            DrawText(
                unsaved_changes_msg.c_str(),
                unsaved_changes_pos.x,
                unsaved_changes_pos.y,
                DEFAULT_TEXT_SIZE,
                DEFAULT_TEXT_COLOR);
        }
    }
};

// Main menu
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
