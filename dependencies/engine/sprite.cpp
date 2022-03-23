#include "sprite.hpp"
#include "utility.hpp"

#include <raylib.h>

Sprite::Sprite(Texture2D* _spritesheet, Rectangle _rect)
    : spritesheet(_spritesheet)
    , rect(_rect) {
}

void Sprite::draw(Vector2 pos) {
    DrawTextureRec(*spritesheet, rect, pos, WHITE);
}

Sprite make_sprite(Texture2D* spritesheet, Rectangle sprite_rect) {
    // I think this will work? Idk how else to ensure if sprite rect is within
    // texture's borders. Maybe I should throw exception instead... TODO.
    ASSERT(is_rect_inside_rect(
        Rectangle{
            0.0f,
            0.0f,
            static_cast<float>(spritesheet->width),
            static_cast<float>(spritesheet->height)},
        sprite_rect));

    return Sprite(spritesheet, sprite_rect);
}

std::vector<Sprite> make_sprites(Texture2D* spritesheet, Vector2 sprite_size) {
    // Ensuring that spritesheet can be cut to sprites of requested size without
    // leftovers.
    // TODO: maybe add some arg to specify some part of spritesheet to ignore.
    ASSERT(spritesheet->width % static_cast<int>(sprite_size.x) == 0);
    ASSERT(spritesheet->height % static_cast<int>(sprite_size.y) == 0);

    int horizontal_amount = spritesheet->width / sprite_size.x;
    int vertical_amount = spritesheet->height / sprite_size.y;

    std::vector<Sprite> sprites;

    // I think this will do? May need to flip these around.
    for (auto current_y = 0; current_y < vertical_amount; current_y++) {
        for (auto current_x = 0; current_x < horizontal_amount; current_x++) {
            float sprite_x = static_cast<float>(sprite_size.x * current_x);
            float sprite_y = static_cast<float>(sprite_size.y * current_y);
            sprites.push_back(Sprite(
                spritesheet,
                // May need to flip the last two
                Rectangle{sprite_x, sprite_y, sprite_size.x, sprite_size.y}));
        }
    }

    return sprites;
}

// Animation stuff

Animation::Animation(
    std::vector<const Texture2D*> _frames, float speed, bool _loop, Vector2 _pos)
    : timer(speed)
    , frames(_frames)
    , loop(_loop)
    , current_frame(0)
    , pos(_pos) {
    timer.start();
}

Animation::Animation(std::vector<const Texture2D*> _frames, float speed, bool _loop)
    : Animation(_frames, speed, _loop, {0, 0}) {
    timer.start();
}

void Animation::update(float dt) {
    if (timer.tick(dt)) {
        if (current_frame + 1 < static_cast<int>(frames.size())) {
            current_frame++;
            timer.start();
        }
        else if (loop) {
            current_frame = 0;
            timer.start();
        }
        else {
            timer.stop();
        }
    };
}

void Animation::draw() {
    DrawTexture(*frames[current_frame], pos.x, pos.y, WHITE);
}
