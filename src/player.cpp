#include "player.h"
#include "game.h"
#include "raylib.h"
#include "sprite_animation.h"
#include "tile.h"

void Player::load()
{
    Image idle_img = LoadImage(RESOURCES_PATH "char_idle.png");
    Texture2D idle_tex = LoadTextureFromImage(idle_img);
    anim_idle.init(idle_tex, 4, 2, 64, 0.25f, 1, true);
    UnloadImage(idle_img);

    Image walk_img = LoadImage(RESOURCES_PATH "char_run.png");
    Texture2D walk_tex = LoadTextureFromImage(walk_img);
    anim_walk.init(walk_tex, 4, 8, 64, 0.12f, 1, true);
    UnloadImage(walk_img);

    Image combat_img = LoadImage(RESOURCES_PATH "char_slash.png");
    Texture2D combat_tex = LoadTextureFromImage(combat_img);
    anim_combat.init(combat_tex, 4, 18, 64, 0.12f, 3, true);
    UnloadImage(combat_img);

    // Set default
    current_anim = &anim_idle;
}

/*void Player::update(float delta, Game& game)
{
    bool moving = false;
    bool combat = false;
    float nextX = pos_x;
    float nextY = pos_y;

    if (IsKeyDown(KEY_A)) {
        nextX -= speed * delta;
        moving = true;
        current_anim->direction = eDirection::Left;
    }
    if (IsKeyDown(KEY_D)) {
        nextX += speed * delta;
        moving = true;
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

    if (IsKeyDown(KEY_SPACE)) {
        combat = true;
        moving = false;
        nextX = pos_x;
        nextY = pos_y;
    }

    Rectangle nextHitbox = {
        nextX + (TILE_WIDTH * 0.25f),
        nextY + TILE_HEIGHT,
        static_cast<float>(TILE_WIDTH * 0.5f),
        static_cast<float>(TILE_HEIGHT)
    };

    if (game.can_move_to(nextHitbox)) {
        pos_x = nextX;
        pos_y = nextY;
        update_tile_index();
    }

    SpriteAnimBase* newAnim = moving ? &anim_walk : &anim_idle;
    if (combat) {
        newAnim = &anim_combat;
        newAnim->direction = current_anim->direction;
    }

    if (newAnim != current_anim) {
        current_anim = newAnim;
        current_anim->frame = 0;
        current_anim->timer = 0.0f;
    }

    current_anim->update(delta, moving);

    update_hitbox();
}*/

void Player::update(float delta, Game& game)
{
    bool moving = false;
    bool combat = false; // local flag for selection below
    float nextX = pos_x;
    float nextY = pos_y;

    if (IsKeyDown(KEY_A)) {
        nextX -= speed * delta;
        moving = true;
        current_anim->direction = eDirection::Left;
    }
    if (IsKeyDown(KEY_D)) {
        nextX += speed * delta;
        moving = true;
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

    static bool combatActive = false; // persists between frames
    bool combatTriggered = IsKeyPressed(KEY_SPACE);

    if (combatTriggered && !combatActive) {
        combatActive = true;
        anim_combat.direction = current_anim->direction;
        anim_combat.frame = 0;
        anim_combat.timer = 0.0f;
    }

    // Movement collision (unchanged)
    Rectangle nextHitbox = {
        nextX + (TILE_WIDTH * 0.25f),
        nextY + TILE_HEIGHT,
        static_cast<float>(TILE_WIDTH * 0.5f),
        static_cast<float>(TILE_HEIGHT)
    };

    if (game.can_move_to(nextHitbox) && !combatActive) {
        pos_x = nextX;
        pos_y = nextY;
        update_tile_index();
    }

    // --- Choose animation (minimal change) ---
    SpriteAnimation* newAnim = moving ? &anim_walk : &anim_idle;

    // If combat was triggered previously and still active, choose combat anim
    if (combatActive) {
        newAnim = &anim_combat;
        // Preserve the locked direction set at trigger time
        // (Do NOT overwrite anim_combat.direction here from current_anim,
        //  otherwise you'll lose the locked direction)
    }

    // Switch only when different (to avoid resetting each frame)
    if (newAnim != current_anim) {
        current_anim = newAnim;
        // Only reset non-combat switches; combat was reset at keypress above
        if (current_anim != &anim_combat) {
            current_anim->frame = 0;
            current_anim->timer = 0.0f;
        }
    }

    current_anim->update(delta, moving);

    // If combatActive and the combat animation finished, clear it so we return to normal animations.
    int frame_limit = anim_combat.row_based ? anim_combat.cols : anim_combat.rows;
    int maxFrames = frame_limit / anim_combat.frame_span;
    if (combatActive && anim_combat.frame >= (maxFrames - 1)) {
        // combat animation reached last frame — stop combat mode
        combatActive = false;
        // optionally reset anim_combat if you want it prepared for next attack:
        anim_combat.frame = 0;
        anim_combat.timer = 0.0f;
    }

    update_hitbox();
}

void Player::draw()
{

    if (current_anim)
        current_anim->draw(pos_x, pos_y, TILE_WIDTH * 2, TILE_HEIGHT * 2);
}

void Player::update_hitbox()
{
    hitbox = {
        static_cast<float>(pos_x + (16 * 0.5f)),
        static_cast<float>(pos_y + 16),
        static_cast<float>(TILE_WIDTH),
        static_cast<float>(TILE_HEIGHT)
    };
}

void Player::update_tile_index()
{
    float feetY = pos_y + TILE_HEIGHT * 2 - (float)TILE_HEIGHT / 2;
    x_index = static_cast<int>((pos_x + (float)TILE_WIDTH / 2) / (float)TILE_WIDTH);
    y_index = static_cast<int>(feetY / TILE_HEIGHT);
}
