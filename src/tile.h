#pragma once
#include <raylib.h>

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;

constexpr int WORLD_WIDTH = 20;
constexpr int WORLD_HEIGHT = 20;

constexpr int TILE_WIDTH = 16;
constexpr int TILE_HEIGHT = 16;

typedef enum {
    TEXTURE_TILEMAP = 0
    // add more textures here if needed
} eTexture_asset;

#define MAX_TEXTURES 10

enum class eTileType {
    FLOOR_0 = 0,
    FLOOR_1,
    FLOOR_2,
    FLOOR_3,
    FLOOR_4,
    HOLE,
    STAIR,
    CHECK,
};

enum class eZone {
    ALL = 0,
    WORLD,
    DUNGEON
};

/*struct Tile {
    int x;
    int y;
    eTileType type;
};*/

struct Tile {
    int x;
    int y;
    int type = -1;
    int textureIndex;
};
