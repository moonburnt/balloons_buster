#include "common.hpp"

#include "app.hpp"

#include <engine/ui.hpp>

GuiBuilder::GuiBuilder(App* app)
    : app(app) {}

Button* GuiBuilder::make_close_button() {
    return new Button(
        app->assets.sprites["cross_default"],
        app->assets.sprites["cross_hover"],
        app->assets.sprites["cross_pressed"],
        app->assets.sounds["button_hover"],
        app->assets.sounds["button_clicked"],
        Rectangle{0, 0, 64, 64});
}

Button* GuiBuilder::make_text_button(std::string txt) {
    return new Button(
        txt,
        app->assets.sprites["button_default"],
        app->assets.sprites["button_hover"],
        app->assets.sprites["button_pressed"],
        app->assets.sounds["button_hover"],
        app->assets.sounds["button_clicked"],
        Rectangle{0, 0, 256, 64});
}

Checkbox* GuiBuilder::make_checkbox(bool default_state) {
    return new Checkbox(
        app->assets.sprites["toggle_on_default"],
        app->assets.sprites["toggle_on_hover"],
        app->assets.sprites["toggle_on_pressed"],
        app->assets.sprites["toggle_off_default"],
        app->assets.sprites["toggle_off_hover"],
        app->assets.sprites["toggle_off_pressed"],
        app->assets.sounds["button_hover"],
        app->assets.sounds["button_clicked"],
        Rectangle{0, 0, 32, 32},
        default_state);
}

// Helper functions
Vector2 get_rand_vec2(int x, int y) {
    return Vector2{
        static_cast<float>(std::rand() % x),
        static_cast<float>(std::rand() % y)};
}

Vector2 get_rand_vec2(Vector2 vec) {
    return get_rand_vec2(vec.x, vec.y);
}

Color get_rand_color() {
    int rgb[3];

    for (int i = 0; i < 3; i++) {
        rgb[i] = std::rand() % 255;
    }

    if (rgb[0] == 0 && rgb[1] == 0 && rgb[2] == 0) {
        rgb[std::rand() % 3] = 10;
    }

    return Color{
        static_cast<unsigned char>(rgb[0]),
        static_cast<unsigned char>(rgb[1]),
        static_cast<unsigned char>(rgb[2]),
        255};
}
