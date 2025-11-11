#include "map.h"
#include "raylib.h"
#include "tile.h"
#include <imgui.h>
#include <random>

Map::Map()
{
}

void Map::init()
{
    Image image = LoadImage(RESOURCES_PATH "dungeon_test.png");
    textures[TEXTURE_TILEMAP] = LoadTextureFromImage(image);
    UnloadImage(image);

    for (int i = 0; i < WORLD_WIDTH; i++) {
        for (int j = 0; j < WORLD_HEIGHT; j++) {
            world[i][j] = { i, j, static_cast<eTileType>(rand() % 5) };

            dungeon[i][j] = { i, j, eTileType::CHECK };
        }
    }
}

Tile& Map::getTile(int x, int y, eZone zone)
{
    if (zone == eZone::WORLD)
        return world[x][y];

    return dungeon[x][y];
}

void Map::draw(eZone zone)
{
    Tile tile;
    int texX = 0;
    int texY = 0;

    for (int i = 0; i < WORLD_WIDTH; i++) {
        for (int j = 0; j < WORLD_HEIGHT; j++) {

            if (zone == eZone::WORLD)
                tile = world[i][j];
            else if (zone == eZone::DUNGEON)
                tile = dungeon[i][j];

            switch (tile.type) {
            case eTileType::FLOOR_0:
                texX = 0;
                texY = 0;
                break;
            case eTileType::FLOOR_1:
                texX = 1;
                texY = 0;
                break;
            case eTileType::FLOOR_2:
                texX = 2;
                texY = 0;
                break;
            case eTileType::FLOOR_3:
                texX = 1;
                texY = 1;
                break;
            case eTileType::FLOOR_4:
                texX = (tile.x % 2 == 0) ? 0 : 1;
                texY = 2;
                break;
            case eTileType::CHECK:
                texX = 7;
                texY = 0;
                break;
            default:
                break;
            }

            draw_tile(tile.x * TILE_WIDTH, tile.y * TILE_HEIGHT, texX, texY);
            // Tile tile = (zone == eZone::WORLD) ? world[i][j] : dungeon[i][j];
            // draw_tile(tile);
        }
    }
}

void Map::draw_tile(int pos_x, int pos_y, int texture_index_x, int texture_index_y)
{
    Rectangle source = { (float)TILE_WIDTH * texture_index_x, (float)TILE_HEIGHT * texture_index_y, (float)TILE_WIDTH, (float)TILE_HEIGHT };
    Rectangle dest = { (float)pos_x, (float)pos_y, (float)TILE_WIDTH, (float)(TILE_HEIGHT) };
    Vector2 origin = { 0, 0 };
    DrawTexturePro(textures[TEXTURE_TILEMAP], source, dest, origin, 0.0f, WHITE);
}

void Map::draw_grid(int width, int height, int tile_w, int tile_h, float line, Color color = { 255, 255, 255, 255 })
{
    for (int x = 0; x <= width; x++) {
        int pos_x = x * tile_w;
        // DrawLine(pos_x, 0, pos_x, height * tile_h, color);
        DrawLineEx(Vector2 { (float)pos_x, 0 }, Vector2 { (float)pos_x, (float)(height * tile_h) }, line, color);
    }

    for (int y = 0; y <= height; y++) {
        int pos_y = y * tile_h;
        // DrawLine(0, pos_y, width * tile_w, pos_y, color);
        DrawLineEx(Vector2 { 0, (float)pos_y }, Vector2 { (float)(width * tile_w), (float)pos_y }, line, color);
    }
}

void Map::draw_tilemap()
{
    Texture2D& tex = textures[TEXTURE_TILEMAP];
    int tilesX = tex.width / 16;
    int tilesY = tex.height / 16;
    const int spacing = 2;
    float scale = 2;

    ImTextureID texID = (ImTextureID)(intptr_t)tex.id;

    ImVec2 startPos = ImGui::GetCursorPos();
    for (int y = 0; y < tilesY; y++) {
        for (int x = 0; x < tilesX; x++) {
            // UV coordinates in texture (normalized)
            ImVec2 uv0 = ImVec2(x * (float)16 / tex.width, y * (float)16 / tex.height);
            ImVec2 uv1 = ImVec2((x + 1) * (float)16 / tex.width, (y + 1) * (float)16 / tex.height);

            // Position in ImGui window
            ImVec2 pos = ImVec2(startPos.x + x * (16 + spacing) * scale,
                startPos.y + y * (16 + spacing) * scale);
            ImGui::SetCursorPos(pos);

            ImGui::Image(texID, ImVec2(16 * scale, 16 * scale), uv0, uv1);
        }

        ImGui::NewLine();
    }
    ImGui::Image(
        (ImTextureID)(intptr_t)textures[TEXTURE_TILEMAP].id,
        ImVec2(256, 256),
        ImVec2(0, 0),
        ImVec2(1, 1));
}
