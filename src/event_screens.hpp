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
    Button* exit_button;

    // Storage for callback functions. Probably should move it somewhere else
    std::unordered_map<std::string, std::function<void()>> callbacks;

public:
    GameoverScreen(std::function<void()> function);
    void update() override;
    void draw() override;
};
