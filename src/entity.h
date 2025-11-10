#pragma once

#include "sprite_animation.h"
#include "tile.h"
#include <raylib.h>
#include <string>

class Entity {
public:
    std::string name;
    int x_index;
    int y_index;
    eZone zone;
    bool is_alive;
    bool is_passable;
    int health;
    int damage;
    int points;
    Rectangle hitbox;
    float scale;

    SpriteAnimation baseAnim;
    bool hasAnimation = false;
    bool show_hitbox = false;

    Entity(std::string n = "", int x = 0, int y = 0, eZone z = eZone::ALL, float s = 1.f, bool pass_through = true)
        : name(std::move(n))
        , x_index(x)
        , y_index(y)
        , scale(s)
        , zone(z)
        , is_alive(true)
        , is_passable(true)
        , health(100)
        , damage(0)
        , points(0)
    {
        update_hitbox();
    }

    virtual ~Entity() = default;

    virtual void update(float delta)
    {
        baseAnim.update(delta, is_alive);
    }

    virtual void draw()
    {
        // Center base animation on the tile
        baseAnim.draw(hitbox.x, hitbox.y, hitbox.width, hitbox.height);
    }

    virtual void update_hitbox()
    {
        hitbox = {
            static_cast<float>(x_index * (float)TILE_WIDTH),
            static_cast<float>(y_index * (float)TILE_WIDTH),
            static_cast<float>(TILE_WIDTH * scale),
            static_cast<float>(TILE_WIDTH * scale)
        };
    }

    void draw_hitbox(Color color = RED) const
    {
        DrawRectangleRec(hitbox, Fade(color, 0.4f));
    }
};
