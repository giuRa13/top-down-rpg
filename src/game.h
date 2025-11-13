#pragma once
#include "editor.h"
#include "entity.h"
#include "entity_registry.h"
#include "map.h"
#include "player.h"
#include "raylib.h"
#include <imgui.h>

#define MAX_SOUNDS 2

inline Sound sounds[MAX_SOUNDS]; // move in header

typedef enum {
    SOUND_ATTACK = 0,
    SOUND_POINTS,
} sound_asset;

enum class eState {
    Game,
    Editor
} inline state
    = eState::Game;

inline bool debugMode = false;

struct GameViewport {
    float x, y;
    float width, height;
};
inline GameViewport viewport = {};

class Game {
public:
    Map map;
    Editor editor;
    Player player;
    Entity* selected_entity = nullptr;
    EntityRegistry entity_registry;

    Camera2D camera;
    Camera2D editor_camera;
    bool free_cam = false;

    RenderTexture2D gameView;
    float viewportOffsetX = 0.0f;
    float viewportOffsetY = 0.0f;
    float viewportWidth = SCREEN_WIDTH;
    float viewportHeight = SCREEN_HEIGHT;

    Game();
    ~Game() = default;

    void game_startup();
    void init_camera();
    void init_editor();
    void update(float delta);
    void draw();
    bool can_move_to(const Rectangle& nextHitbox);
    void handle_entity_selection();

    void draw_overlay();
    void draw_entity_panel();
    void draw_ui();
    void draw_mouse_highlight();
};
