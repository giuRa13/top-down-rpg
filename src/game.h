#pragma once
#include "entity.h"
#include "map.h"
#include "player.h"
#include "raylib.h"
#include "tile.h"
#include <imgui.h>
#include <memory>
#include <rlImgui.h>
#include <string>
#include <unordered_map>
#include <vector>

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;

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
    Player player;
    std::vector<std::unique_ptr<Entity>> entities;
    std::unordered_map<std::string, Entity*> entity_map;

    Camera2D camera;
    Camera2D editor_camera;

    RenderTexture2D gameView;
    float viewportOffsetX = 0.0f;
    float viewportOffsetY = 0.0f;
    float viewportWidth = SCREEN_WIDTH;
    float viewportHeight = SCREEN_HEIGHT;

    Game();
    ~Game() = default;

    void game_startup();
    void init_camera();
    void update(float delta);
    void draw();
    bool can_move_to(const Rectangle& nextHitbox);

    void draw_overlay();
    void draw_entity_panel();
    void draw_ui();
    void draw_mouse_highlight();

    template <typename T, typename... Args>
    T* spawn_entity(const std::string& name, Args&&... args)
    {
        auto e = std::make_unique<T>(name, std::forward<Args>(args)...);
        T* ptr = e.get();
        entity_map[name] = ptr;
        entities.push_back(std::move(e));
        return ptr;
    }

    Entity* get_entity(const std::string& name)
    {
        auto it = entity_map.find(name);
        return (it != entity_map.end()) ? it->second : nullptr;
    }
};
