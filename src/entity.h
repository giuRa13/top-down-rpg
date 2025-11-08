#pragma once

#include "tile.h"
#include <raylib.h>

class Entity {
public:
    int x_index;
    int y_index;
    eZone zone;
    bool is_alive;
    bool is_passable;
    int health;
    int damage;
    int points;
    Rectangle hitbox;

    Entity(int x = 0, int y = 0, eZone z = eZone::ALL, bool pass_through = true)
        : x_index(x)
        , y_index(y)
        , zone(z)
        , is_alive(true)
        , is_passable(true)
        , health(100)
        , damage(0)
        , points(0)
    {
        update_hitbox();
    }

    virtual void update_hitbox()
    {
        hitbox = {
            static_cast<float>(x_index * (float)TILE_WIDTH),
            static_cast<float>(y_index * (float)TILE_HEIGHT),
            static_cast<float>(TILE_WIDTH),
            static_cast<float>(TILE_HEIGHT)
        };
    }

    void draw_hitbox(Color color = RED) const
    {
        DrawRectangleRec(hitbox, Fade(color, 0.4f));
    }
};
