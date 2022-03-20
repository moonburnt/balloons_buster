#include "ui.hpp"
#include "utility.hpp"

#include <optional>
#include <raylib.h>
#include <string>
#include <tuple>

// Label
Label::Label(std::string txt, Vector2 position)
    : text(txt)
    , pos(position)
    , real_pos(position) {
}

Label::Label(std::string txt, int x, int y)
    : Label(txt, Vector2{static_cast<float>(x), static_cast<float>(y)}) {
}

Label::Label()
    : Label("", Vector2{0, 0}) {
}

void Label::center() {
    real_pos = center_text(text, pos);
}

void Label::set_pos(Vector2 _pos, bool _center) {
    pos = _pos;
    if (_center) center();
    else real_pos = pos;
}

void Label::set_pos(Vector2 _pos) {
    set_pos(_pos, false);
}

Vector2 Label::get_pos() {
    return pos;
}

void Label::set_text(std::string txt) {
    text = txt;
}

void Label::draw() {
    DrawText(text.c_str(), real_pos.x, real_pos.y, DEFAULT_TEXT_SIZE, DEFAULT_TEXT_COLOR);
}

// It's not necessary to use "this" in these, but it may be good for readability
void Button::reset_state() {
    state = ButtonStates::idle;
    last_state = ButtonStates::idle;
}

// Constructors dont need to specify return state
Button::Button(
    const Texture2D* texture_default,
    const Texture2D* texture_hover,
    const Texture2D* texture_pressed,
    const Sound* sfx_hover,
    const Sound* sfx_click,
    Rectangle rectangle) {
    textures[ButtonStates::idle] = texture_default;
    textures[ButtonStates::hover] = texture_hover;
    textures[ButtonStates::pressed] = texture_pressed;
    textures[ButtonStates::clicked] = texture_default;
    sounds[0] = sfx_hover;
    sounds[1] = sfx_click;
    rect = rectangle;
    pos = Vector2{0, 0};
    manual_update_mode = false;
    reset_state();
}

enum ButtonStates Button::update() {
    if (manual_update_mode) return state;

    if (CheckCollisionPointRec(GetMousePosition(), rect)) {
        if (last_state == ButtonStates::pressed) {
            if (IsMouseButtonUp(MOUSE_BUTTON_LEFT)) {
                state = ButtonStates::clicked;
                PlaySound(*sounds[1]);
            }
        }
        else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) state = ButtonStates::pressed;

        else {
            if (last_state != ButtonStates::hover) PlaySound(*sounds[0]);
            state = ButtonStates::hover;
        }
    }
    else state = ButtonStates::idle;

    last_state = state;

    return state;
}

void Button::draw() {
    DrawTexture(*textures[state], pos.x, pos.y, WHITE);
}

void Button::set_state(ButtonStates _state) {
    if (_state == ButtonStates::clicked) PlaySound(*sounds[1]);
    else if (_state == ButtonStates::hover) PlaySound(*sounds[0]);

    state = _state;
    last_state = state;
}

ButtonStates Button::get_state() {
    return state;
}

void Button::set_manual_update_mode(bool mode) {
    manual_update_mode = mode;
}

void Button::set_pos(Vector2 position) {
    // Hitbox should always retain initial position diff

    int x_diff = rect.x - pos.x;
    int y_diff = rect.y - pos.y;

    pos.x = position.x;
    pos.y = position.y;

    rect.x = position.x + x_diff;
    rect.y = position.y + y_diff;
}

Vector2 Button::get_pos() {
    return pos;
}

Rectangle Button::get_rect() {
    return rect;
}

// Getter for private clicked var. This allows to make it readable, but prevent
// overwriting from outside
bool Button::is_clicked() {
    return state == ButtonStates::clicked;
}

// This is how we call parent's constructor from child constructor, with passing
// required arguments to it. Parent's constructor will be solved after child.
TextButton::TextButton(
    const Texture2D* texture_default,
    const Texture2D* texture_hover,
    const Texture2D* texture_pressed,
    const Sound* sfx_hover,
    const Sound* sfx_click,
    Rectangle rectangle,
    std::string msg,
    Vector2 msg_pos)
    : Button(
          texture_default,
          texture_hover,
          texture_pressed,
          sfx_hover,
          sfx_click,
          rectangle)
    , text(Label(msg, msg_pos)) {
}

TextButton::TextButton(
    const Texture2D* texture_default,
    const Texture2D* texture_hover,
    const Texture2D* texture_pressed,
    const Sound* sfx_hover,
    const Sound* sfx_click,
    Rectangle rectangle,
    std::string msg)
    : TextButton(
          texture_default,
          texture_hover,
          texture_pressed,
          sfx_hover,
          sfx_click,
          rectangle,
          msg,
          // I'm not sure if this should be based on center of rect or on center of
          // texture. For now it's done like that, may change in future
          center_text(
              msg,
              Vector2{texture_default->width / 2.0f, texture_default->height / 2.0f})) {
}

void TextButton::set_text(std::string txt) {
    text.set_text(txt);
    text.center();
}

void TextButton::draw() {
    Button::draw();
    text.draw();
}

void TextButton::set_pos(Vector2 position) {
    Button::set_pos(position);
    text.set_pos(
        Vector2{
            position.x + (textures[ButtonStates::idle]->width / 2.0f),
            position.y + (textures[ButtonStates::idle]->height / 2.0f)},
        true);
}

// Checkbox shenanigans
Checkbox::Checkbox(
    const Texture2D* texture_on_default,
    const Texture2D* texture_on_hover,
    const Texture2D* texture_on_pressed,
    const Texture2D* texture_off_default,
    const Texture2D* texture_off_hover,
    const Texture2D* texture_off_pressed,
    const Sound* sfx_hover,
    const Sound* sfx_click,
    Rectangle rectangle,
    bool default_state)
    : Button(
          texture_on_default,
          texture_on_hover,
          texture_on_pressed,
          sfx_hover,
          sfx_click,
          rectangle) {
    textures_off[ButtonStates::idle] = texture_off_default,
    textures_off[ButtonStates::hover] = texture_off_hover,
    textures_off[ButtonStates::pressed] = texture_off_pressed,
    textures_off[ButtonStates::clicked] = texture_off_default,
    toggle_state = default_state;
    state_switched = false;
}

Checkbox::Checkbox(
    const Texture2D* texture_on_default,
    const Texture2D* texture_on_hover,
    const Texture2D* texture_on_pressed,
    const Texture2D* texture_off_default,
    const Texture2D* texture_off_hover,
    const Texture2D* texture_off_pressed,
    const Sound* sfx_hover,
    const Sound* sfx_click,
    Rectangle rectangle)
    : Checkbox(
          texture_on_default,
          texture_on_hover,
          texture_on_pressed,
          texture_off_default,
          texture_off_hover,
          texture_off_pressed,
          sfx_hover,
          sfx_click,
          rectangle,
          true) {
}

bool Checkbox::get_toggle() {
    return toggle_state;
}

void Checkbox::toggle(bool _toggle_state) {
    Button::reset_state();
    // if (toggle_state != _toggle_state) state_switched = true;
    toggle_state = _toggle_state;
}

void Checkbox::toggle() {
    if (toggle_state) toggle(false);
    else toggle(true);
    if (state_switched) state_switched = false;
    else state_switched = true;
}

void Checkbox::draw() {
    if (toggle_state) Button::draw();
    else DrawTexture(*textures_off[state], pos.x, pos.y, WHITE);
}

ButtonStates Checkbox::update() {
    Button::update();
    if (Button::is_clicked()) toggle();
    return state;
}

bool Checkbox::is_clicked() {
    return state_switched;
}

void Checkbox::reset_state() {
    Button::reset_state();
    state_switched = false;
}

// Button Storage
ButtonStorage::ButtonStorage() {
    selected_button = -1;
    manual_update_mode = false;
}

ButtonStorage::~ButtonStorage() {
    // This may be not the right way to do things, idk
    for (auto i : storage) {
        delete i;
    }
}

ButtonBase* ButtonStorage::operator[](int i) {
    return storage[i];
}

void ButtonStorage::add_button(ButtonBase* button) {
    storage.push_back(button);
    if (manual_update_mode) button->set_manual_update_mode(true);
}

void ButtonStorage::set_manual_update_mode(bool mode) {
    if (mode == manual_update_mode) return;

    if (mode) {
        for (auto i : storage) {
            i->set_manual_update_mode(true);
            i->reset_state();
        }
    }
    else {
        for (auto i : storage) {
            i->set_manual_update_mode(false);
            i->reset_state();
        }
    }

    manual_update_mode = mode;
}

void ButtonStorage::select_button(size_t button) {
    if (!storage.empty() && button >= 0 && button < storage.size()) {
        set_manual_update_mode(true);
        if (selected_button && selected_button != -1ul) {
            storage[selected_button]->reset_state();
        }
        selected_button = button;
        storage[selected_button]->set_state(ButtonStates::hover);
    }
}

void ButtonStorage::select_next(bool cycle) {
    int button;
    if (cycle && (selected_button + 1ul == storage.size())) {
        button = 0;
    }
    else {
        button = selected_button + 1;
    }

    select_button(button);
}

void ButtonStorage::select_previous(bool cycle) {
    int button;
    if (cycle && (selected_button - 1ul < 0)) {
        // TODO: may need to set it to size-1.
        button = storage.size();
    }
    else {
        button = selected_button - 1;
    }

    select_button(button);
}

void ButtonStorage::update() {
    if (manual_update_mode) {
        if (selected_button >= 0) {
            storage[selected_button]->update();
        }
    }
    else {
        for (auto button : storage) {
            button->update();
        }
    }
}

std::optional<std::tuple<int, ButtonStates>> ButtonStorage::get_selected_button_state() {
    if (selected_button >= 0) {
        return std::make_tuple(selected_button, storage[selected_button]->get_state());
    }

    return std::nullopt;
}

bool ButtonStorage::set_selected_button_state(ButtonStates state) {
    if (selected_button < 0) return false;

    storage[selected_button]->set_state(state);

    return true;
}

void ButtonStorage::draw() {
    for (auto button : storage) {
        button->draw();
    }
}