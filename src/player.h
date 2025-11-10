#pragma once

#include "entity.h"
#include "sprite_animation.h"
#include "tile.h"
#include <string>

class Game;

class Player : public Entity {

public:
    float pos_x;
    float pos_y;
    float speed;
    SpriteAnimation anim_idle;
    SpriteAnimation anim_walk;
    SpriteAnimation anim_combat;
    SpriteAnimation* current_anim;
    bool flip;

    Player(int start_x, int start_y, eZone z)
        : Entity("player", start_x, start_y, z)
        , pos_x(start_x * TILE_WIDTH)
        , pos_y(start_y * TILE_HEIGHT)
        , speed(120.0f)
        , current_anim(nullptr)
        , flip(false)
    {
    }

    void load();
    void update(float delta, Game& game);
    void draw() override;
    void update_hitbox() override;

private:
    void update_tile_index();
};
