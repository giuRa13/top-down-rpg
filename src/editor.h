#pragma once

#include "tile.h"
#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>
#include <string>

class Map;

class Editor {

public:
    Editor(Map& map)
        : map(map)
    {
    }

    int selected_texture_index = 0;
    int tile_width = TILE_WIDTH;
    int tile_height = TILE_HEIGHT;
    // Number of tiles in x/y in the tilemap texture
    int tiles_x = 0;
    int tiles_y = 0;
    // Currently selected tile index
    int selected_index_x = 0;
    int selected_index_y = 0;
    bool cancel_tile_mode = false;
    bool fill_all_mode = false;

    bool saveDialogOpen = false;
    bool loadDialogOpen = false;
    std::string currentFilePath;
    Map& map;
    int selectedTextureIndex = 0;

    void load_tilemap(Texture2D& tex, int tileW, int tileH);
    void draw_tilemap_panel();
    void draw_editor_bar();

    void reset_map();
    void open_save_as_dialog();
    void open_load_dialog();

    Rectangle get_selected_tile_rect() const;
};
