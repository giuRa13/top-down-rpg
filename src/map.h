#include "tile.h"
#include <raylib.h>

class Map {

public:
    Map();
    void init();
    void draw(eZone zone);
    Tile& getTile(int x, int y, eZone zone);

    void draw_tile(int pos_x, int pos_y, int texture_index_x, int texture_index_y);
    void draw_grid(int w, int h, int tile_w, int tile_h, Color color);
    void draw_tilemap();

private:
    Tile world[WORLD_WIDTH][WORLD_HEIGHT];
    Tile dungeon[WORLD_WIDTH][WORLD_HEIGHT];
    Texture2D textures[1];
};
