#include "common.hpp"
// For rand()
#include <cstdlib>

Button* make_close_button() {
    return new Button(
        shared::assets.sprites["cross_default"],
        shared::assets.sprites["cross_hover"],
        shared::assets.sprites["cross_pressed"],
        shared::assets.sounds["button_hover"],
        shared::assets.sounds["button_clicked"],
        Rectangle{0, 0, 64, 64});
}

Button* make_text_button(std::string txt) {
    return new Button(
        txt,
        shared::assets.sprites["button_default"],
        shared::assets.sprites["button_hover"],
        shared::assets.sprites["button_pressed"],
        shared::assets.sounds["button_hover"],
        shared::assets.sounds["button_clicked"],
        Rectangle{0, 0, 256, 64});
}

Checkbox* make_checkbox(bool default_state) {
    return new Checkbox(
        shared::assets.sprites["toggle_on_default"],
        shared::assets.sprites["toggle_on_hover"],
        shared::assets.sprites["toggle_on_pressed"],
        shared::assets.sprites["toggle_off_default"],
        shared::assets.sprites["toggle_off_hover"],
        shared::assets.sprites["toggle_off_pressed"],
        shared::assets.sounds["button_hover"],
        shared::assets.sounds["button_clicked"],
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
