#include "player.h"
#include "game.h"

void Player::load()
{
    Image idle_img = LoadImage(RESOURCES_PATH "char_idle.png");
    Texture2D idle_tex = LoadTextureFromImage(idle_img);
    anim_idle.init(idle_tex, 2, 3, 3, 0.25f);
    UnloadImage(idle_img);

    Image walk_img = LoadImage(RESOURCES_PATH "char_run.png");
    Texture2D walk_tex = LoadTextureFromImage(walk_img);
    anim_walk.init(walk_tex, 8, 3, 8, 0.12f);
    UnloadImage(walk_img);

    // Set default
    current_anim = &anim_idle;
}

void Player::update(float delta, Game& game)
{
    bool moving = false;
    float nextX = pos_x;
    float nextY = pos_y;

    /*if (IsKeyDown(KEY_A)) {
        pos_x -= speed * delta;
        current_anim->direction = eDirection::Right,
        flip = true;
        current_anim->flip = flip;
        moving = true;
    } else if (IsKeyDown(KEY_D)) {
        pos_x += speed * delta;
        current_anim->direction = eDirection::Right,
        flip = false;
        current_anim->flip = flip;
        moving = true;
    } else if (IsKeyDown(KEY_W)) {
        pos_y -= speed * delta;
        current_anim->direction = eDirection::Up,
        moving = true;
    } else if (IsKeyDown(KEY_S)) {
        pos_y += speed * delta;
        current_anim->direction = eDirection::Down,
        moving = true;
    }*/
    if (IsKeyDown(KEY_A)) {
        nextX -= speed * delta;
        moving = true;
        flip = true;
        current_anim->direction = eDirection::Right;
    }
    if (IsKeyDown(KEY_D)) {
        nextX += speed * delta;
        moving = true;
        flip = false;
        current_anim->direction = eDirection::Right;
    }
    if (IsKeyDown(KEY_W)) {
        nextY -= speed * delta;
        moving = true;
        current_anim->direction = eDirection::Up;
    }
    if (IsKeyDown(KEY_S)) {
        nextY += speed * delta;
        moving = true;
        current_anim->direction = eDirection::Down;
    }

    Rectangle nextHitbox = {
        nextX + TILE_WIDTH * 0.5f,
        nextY,
        static_cast<float>(TILE_WIDTH),
        static_cast<float>(TILE_HEIGHT)
    };

    if (game.can_move_to(nextHitbox)) {
        pos_x = nextX;
        pos_y = nextY;
        update_tile_index();
    }

    current_anim = moving ? &anim_walk : &anim_idle;
    // if (current_anim)
    // current_anim->update(moving);

    // update_tile_index();
    if (current_anim)
        current_anim->flip = flip, current_anim->update(moving);
    update_hitbox();
}

void Player::draw()
{
    if (current_anim)
        current_anim->draw(pos_x, pos_y, TILE_WIDTH * 2, TILE_HEIGHT * 2);

    // draw_hitbox(RED);
}

void Player::update_hitbox()
{
    hitbox = {
        static_cast<float>(pos_x + (16 * 0.5f)),
        static_cast<float>(pos_y),
        static_cast<float>(TILE_WIDTH),
        static_cast<float>(TILE_HEIGHT)
    };
}

void Player::update_tile_index()
{
    float feetY = pos_y + TILE_HEIGHT * 2 - TILE_HEIGHT / 2;
    x_index = static_cast<int>((pos_x + TILE_WIDTH / 2) / TILE_WIDTH);
    y_index = static_cast<int>((feetY / TILE_HEIGHT) - 1);
}
