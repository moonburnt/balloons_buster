#pragma once

#include "engine/ui.hpp"
#include "raylib.h"

#include <functional>
#include <string>
#include <unordered_map>

class App;

// This may be an overkill, but its starting to get hard to get through level's
// items, so I've needed to move these primitives somewhere
class EventScreen {
protected:
    Rectangle bg;
    Color bg_color;
    App* app;

public:
    EventScreen(App* app, Rectangle bg, Color bg_color);
    virtual void update() = 0;
    virtual void draw() = 0;
    virtual ~EventScreen() = default;
};

class GameoverScreen : public EventScreen {
private:
    Label title_label;
    Label body_label;

    VerticalContainer buttons;

public:
    GameoverScreen(App* app, std::string title, std::string body, std::function<void()> exit_func);

    void set_body_text(std::string txt);

    void update() override;
    void draw() override;
};

class PauseScreen : public EventScreen {
private:
    Label title_label;
    VerticalContainer buttons;

public:
    PauseScreen(
        App* app, std::string title, std::function<void()> cont_func,
        std::function<void()> exit_func);

    void update() override;
    void draw() override;
};
