#include "event_screens.hpp"
#include "level.hpp"
#include "raylib.h"
// For basic formatting
#include "common.hpp"
#include <fmt/core.h>
#include <tuple>

static constexpr Color SIDE_BG_COLOR{203, 219, 252, 255};
static constexpr Color CORNER_COLOR{34, 32, 52, 255};

EventScreen::EventScreen(Rectangle _bg, Color _bg_color)
    : bg(_bg)
    , bg_color(_bg_color) {
}

GameoverScreen::GameoverScreen(std::string title, std::string body, std::function<void()> function)
    : EventScreen(
          Rectangle{
              ((GetScreenWidth() - GetScreenHeight()) / 2.0f + 30),
              30,
              (GetScreenWidth() + 30) / 2.0f,
              (GetScreenHeight() - 60.0f)},
          {0, 0, 0, 0})
    , title_label(title, {GetScreenWidth() / 2.0f, 130.0f})
    , body_label(body, {GetScreenWidth() / 2.0f, 200.0f})
    , exit_button(make_text_button("Back to Menu")) {
    callbacks["call_exit"] = function;
    title_label.center();
    body_label.center();
    exit_button->set_pos(Vector2{
        GetScreenWidth() / 2.0f - exit_button->get_rect().width / 2,
        GetScreenHeight() / 2.0f + 100});
}

void GameoverScreen::update() {
    exit_button->update();

    if (exit_button->is_clicked()) {
        callbacks["call_exit"]();
        return;
    }
}

void GameoverScreen::draw() {
    DrawRectangleRec(bg, SIDE_BG_COLOR);
    DrawRectangleLinesEx(bg, 1.0f, CORNER_COLOR);

    title_label.draw();
    body_label.draw();
    exit_button->draw();
}
