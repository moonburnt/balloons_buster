#pragma once

#include "engine/ui.hpp"
#include "raylib.h"

#include <functional>
#include <string>
#include <unordered_map>

// This may be an overkill, but its starting to get hard to get through level's
// items, so I've needed to move these primitives somewhere
class EventScreen {
protected:
    Rectangle bg;
    Color bg_color;

public:
    EventScreen(Rectangle bg, Color bg_color);
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
    GameoverScreen(std::string title, std::string body, std::function<void()> exit_func);

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
        std::string title, std::function<void()> cont_func, std::function<void()> exit_func);

    void update() override;
    void draw() override;
};
