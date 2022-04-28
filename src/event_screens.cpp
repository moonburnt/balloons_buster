#include "event_screens.hpp"

#include "app.hpp"
#include "common.hpp"
#include "level.hpp"

#include <fmt/core.h>

#include <tuple>

#include <raylib.h>

static constexpr Color SIDE_BG_COLOR{203, 219, 252, 255};
static constexpr Color CORNER_COLOR{34, 32, 52, 255};

EventScreen::EventScreen(App* app, Rectangle _bg, Color _bg_color)
    : bg(_bg)
    , bg_color(_bg_color)
    , app(app) {}

GameoverScreen::GameoverScreen(App* app, std::string title, std::string body, std::function<void()> exit_func)
    : EventScreen(
        app,
        Rectangle{
            ((get_window_width() - get_window_height()) / 2.0f + 30),
            30,
            (get_window_width() + 30) / 2.0f,
            (get_window_height() - 60.0f)},
        {0, 0, 0, 0})
    , title_label(title, {get_window_width() / 2.0f, 130.0f})
    , body_label(body, {get_window_width() / 2.0f, 200.0f})
    , buttons(32.0f) {
    title_label.center();
    body_label.center();

    GuiBuilder builder(app);
    Button* exit_button = builder.make_text_button("Back to Menu");
    exit_button->set_callback(exit_func);

    buttons.add_button(exit_button);

    buttons.set_pos({get_window_width() / 2.0f, get_window_height() / 2.0f});
    buttons.center();
}

void GameoverScreen::set_body_text(std::string txt) {
    body_label.set_text(txt);
    body_label.center();
}

void GameoverScreen::update() {
    buttons.update();
}

void GameoverScreen::draw() {
    DrawRectangleRec(bg, SIDE_BG_COLOR);
    DrawRectangleLinesEx(bg, 1.0f, CORNER_COLOR);

    title_label.draw();
    body_label.draw();
    buttons.draw();
}

// Pause screen

PauseScreen::PauseScreen(
    App* app,
    std::string title, std::function<void()> cont_func,
    std::function<void()> exit_func)
    : EventScreen(
        app,
        Rectangle{
            ((get_window_width() - get_window_height()) / 2.0f + 30),
            30,
            (get_window_width() + 30) / 2.0f,
            (get_window_height() - 60.0f)},
        {0, 0, 0, 0})
    , title_label(title, {get_window_width() / 2.0f, 130.0f})
    , buttons(32.0f) {

    title_label.center();

    GuiBuilder builder(app);
    Button* cont_button = builder.make_text_button("Continue");
    cont_button->set_callback(cont_func);
    Button* exit_button = builder.make_text_button("Back to Menu");
    exit_button->set_callback(exit_func);

    buttons.add_button(cont_button);
    buttons.add_button(exit_button);

    buttons.set_pos({get_window_width() / 2.0f, get_window_height() / 2.0f});
    buttons.center();
}

void PauseScreen::update() {
    buttons.update();
}

void PauseScreen::draw() {
    DrawRectangleRec(bg, SIDE_BG_COLOR);
    DrawRectangleLinesEx(bg, 1.0f, CORNER_COLOR);

    title_label.draw();
    buttons.draw();
}
