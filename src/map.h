#ifndef MAP_H
#define MAP_H

#include "editor.h"
#include "tile.h"
#include <raylib.h>
#include <string>
#include <vector>

struct EditorViewport {
    float x, y;
    float width, height;
};

class Map {

public:
    Map();
    void init();
    // void draw(eZone zone);
    void draw();
    Tile& getTile(int x, int y, eZone zone);

    Texture2D* get_texture_by_name(const std::string& name);
    void draw_tile(int pos_x, int pos_y, int texture_index_x, int texture_index_y);
    void draw_tile(int pos_x, int pos_y, int texture_index_x, int texture_index_y, Texture2D& tex);
    void draw_tile(int pos_x, int pos_y, int tex_x, int tex_y, const std::string& textureName);

    void draw_grid(int w, int h, int tile_w, int tile_h, float line, Color color);
    void draw_tilemap_previews(Editor& editor);
    void draw_editor_map(const EditorViewport& viewport, Editor& editor, Camera2D& cam);

    Tile editor_map[WORLD_WIDTH][WORLD_HEIGHT];
    // Texture2D textures[MAX_TEXTURES];
    // int textureCount = 0;
    std::vector<Texture2D> textures;
    std::vector<std::string> textureNames;
    std::vector<std::string> missingTextures;
    bool showMissingTexturesModal = false;

    int add_texture(const std::string& path);
    void load_tilemaps(const std::string& folder_path);
    bool save_to_file(const std::string& path);
    bool load_from_file(const std::string& path);

private:
    Tile world[WORLD_WIDTH][WORLD_HEIGHT];
    Tile dungeon[WORLD_WIDTH][WORLD_HEIGHT];
};

#endif
