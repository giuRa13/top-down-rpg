#include "map.h"
#include "editor.h"
#include "raylib.h"
#include "tile.h"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <imgui.h>
#include <random>

Map::Map()
{
}

void Map::init()
{
    /*Image image = LoadImage(RESOURCES_PATH "dungeon_test.png");
    textures[TEXTURE_TILEMAP] = LoadTextureFromImage(image);
    UnloadImage(image);*/

    load_tilemaps(RESOURCES_PATH "tilemaps/");

    for (int i = 0; i < WORLD_WIDTH; i++) {
        for (int j = 0; j < WORLD_HEIGHT; j++) {
            // world[i][j] = { i, j, static_cast<eTileType>(rand() % 5) };
            // dungeon[i][j] = { i, j, eTileType::CHECK };
            editor_map[i][j] = { i, j, -1, 0 };
        }
    }
}

void Map::load_tilemaps(const std::string& folder_path)
{
    for (const auto& entry : std::filesystem::directory_iterator(folder_path)) {
        if (entry.path().extension() == ".png") {
            std::string path = entry.path().string();
            int idx = add_texture(path);
            if (idx >= 0) {
                TraceLog(LOG_INFO, "Loaded tilemap: %s", path.c_str());
            }
        }
    }
}

Texture2D* Map::get_texture_by_name(const std::string& name)
{
    for (size_t i = 0; i < textureNames.size(); ++i) {
        if (textureNames[i].find(name) != std::string::npos) // partial match is fine
            return &textures[i];
    }
    TraceLog(LOG_WARNING, "Texture not found by name: %s", name.c_str());
    return nullptr;
}

Tile& Map::getTile(int x, int y, eZone zone)
{
    if (zone == eZone::WORLD)
        return world[x][y];

    return dungeon[x][y];
}

void Map::draw()
{
    for (int x = 0; x < WORLD_WIDTH; ++x) {
        for (int y = 0; y < WORLD_HEIGHT; ++y) {
            Tile& t = editor_map[x][y];
            if (t.type < 0 || t.textureIndex < 0 || t.textureIndex >= textureCount)
                continue;

            Texture2D& tex = textures[t.textureIndex]; // Use correct texture
            int tilesX = tex.width / TILE_WIDTH; // Tiles per row

            int texX = t.type % tilesX;
            int texY = t.type / tilesX;

            draw_tile(x * TILE_WIDTH, y * TILE_HEIGHT, texX, texY, tex);
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

void Map::draw_tile(int pos_x, int pos_y, int tex_x, int tex_y, const std::string& textureName)
{
    Texture2D* tex = get_texture_by_name(textureName);
    if (!tex)
        return;

    Rectangle source = {
        (float)(tex_x * TILE_WIDTH),
        (float)(tex_y * TILE_HEIGHT),
        (float)TILE_WIDTH,
        (float)TILE_HEIGHT
    };
    Rectangle dest = { (float)pos_x, (float)pos_y, (float)TILE_WIDTH, (float)TILE_HEIGHT };
    Vector2 origin = { 0, 0 };

    DrawTexturePro(*tex, source, dest, origin, 0.f, WHITE);
}

void Map::draw_tile(int pos_x, int pos_y, int tile_index_x, int tile_index_y, Texture2D& tex)
{
    Rectangle source = {
        (float)(tile_index_x * TILE_WIDTH),
        (float)(tile_index_y * TILE_HEIGHT),
        (float)TILE_WIDTH,
        (float)TILE_HEIGHT
    };
    Rectangle dest = { (float)pos_x, (float)pos_y, (float)TILE_WIDTH, (float)TILE_HEIGHT };
    Vector2 origin = { 0, 0 };
    DrawTexturePro(tex, source, dest, origin, 0.f, WHITE);
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

void Map::draw_editor_map(const EditorViewport& viewport, Editor& editor, Camera2D& cam)
{
    DrawRectangle(0, 0, TILE_WIDTH * WORLD_WIDTH, TILE_HEIGHT * WORLD_HEIGHT, DARKGRAY);
    draw_grid(WORLD_WIDTH, WORLD_HEIGHT, TILE_WIDTH, TILE_HEIGHT, 1.0f, BLACK);

    // Draw placed tiles from editor_map
    for (int x = 0; x < WORLD_WIDTH; ++x) {
        for (int y = 0; y < WORLD_HEIGHT; ++y) {
            Tile& t = editor_map[x][y];
            if (t.type < 0 || t.textureIndex < 0 || t.textureIndex >= textureCount)
                continue;

            // Use the correct texture for this tile
            Texture2D& tex = textures[t.textureIndex]; // assumes Tile stores textureIndex
            int tilesX = tex.width / TILE_WIDTH; // tiles per row in this texture
            int texX = t.type % tilesX;
            int texY = t.type / tilesX;
            draw_tile(x * TILE_WIDTH, y * TILE_HEIGHT, texX, texY, tex);
        }
    }

    ImVec2 mousePos = ImGui::GetMousePos();
    float mouseX = mousePos.x - viewport.x;
    float mouseY = mousePos.y - viewport.y;

    if (mouseX < 0 || mouseY < 0 || mouseX > viewport.width || mouseY > viewport.height)
        return;

    float scaleX = (float)SCREEN_WIDTH / viewport.width;
    float scaleY = (float)SCREEN_HEIGHT / viewport.height;
    Vector2 mouseWorld = GetScreenToWorld2D({ mouseX * scaleX, mouseY * scaleY }, cam);

    int tileX = (int)(mouseWorld.x / TILE_WIDTH);
    int tileY = (int)(mouseWorld.y / TILE_HEIGHT);

    // if (tileX >= 0 && tileY >= 0 && tileX < WORLD_WIDTH && tileY < WORLD_HEIGHT) {
    int selIndex = editor.selected_index_y * editor.tiles_x + editor.selected_index_x;
    Texture2D& selTex = textures[editor.selectedTextureIndex];
    int selX = editor.selected_index_x;
    int selY = editor.selected_index_y;

    if (!editor.cancel_tile_mode)
        draw_tile(tileX * TILE_WIDTH, tileY * TILE_HEIGHT, selX, selY, selTex);
    else if (editor.cancel_tile_mode)
        DrawRectangle(tileX * TILE_WIDTH, tileY * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, Fade(RED, 0.4f));

    if (tileX >= 0 && tileY >= 0 && tileX < WORLD_WIDTH && tileY < WORLD_HEIGHT) {
        if (editor.cancel_tile_mode && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            editor_map[tileX][tileY].type = -1; // remove tile type
            editor_map[tileX][tileY].textureIndex = -1; // remove texture
        } else if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            editor_map[tileX][tileY].type = editor.selected_index_y * editor.tiles_x + editor.selected_index_x;
            editor_map[tileX][tileY].textureIndex = editor.selectedTextureIndex;
        }

        if (editor.fill_all_mode) {

            // DrawRectangle(0, 0, TILE_WIDTH * WORLD_WIDTH, TILE_HEIGHT * WORLD_HEIGHT, Fade(GREEN, 0.4f));
            for (int x = 0; x < WORLD_WIDTH; ++x) {
                for (int y = 0; y < WORLD_HEIGHT; ++y) {
                    draw_tile(x * TILE_WIDTH, y * TILE_HEIGHT, selX, selY, selTex);
                    DrawRectangle(x * TILE_WIDTH, y * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, Fade(GREEN, 0.3f));
                }
            }

            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                for (int x = 0; x < WORLD_WIDTH; ++x) {
                    for (int y = 0; y < WORLD_HEIGHT; ++y) {
                        editor_map[x][y].type = editor.selected_index_y * editor.tiles_x + editor.selected_index_x;
                        editor_map[x][y].textureIndex = editor.selectedTextureIndex;
                    }
                }
            }
        }
    }
    //}
}

int Map::add_texture(const std::string& path)
{
    Image img = LoadImage(path.c_str());
    if (img.data == nullptr) {
        TraceLog(LOG_ERROR, "Failed to load texture: %s", path.c_str());
        return -1;
    }

    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);

    if (textureCount >= MAX_TEXTURES) {
        TraceLog(LOG_WARNING, "No empty slot for new texture");
        UnloadTexture(tex);
        return -1;
    }

    textures[textureCount] = tex;
    textureNames.push_back(path);
    return textureCount++;
}

bool Map::save_to_file(const std::string& path)
{
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        TraceLog(LOG_ERROR, "Failed to open file for writing: %s", path.c_str());
        return false;
    }

    for (int y = 0; y < WORLD_HEIGHT; ++y) {
        for (int x = 0; x < WORLD_WIDTH; ++x) {
            Tile& t = editor_map[x][y];
            file.write(reinterpret_cast<char*>(&t.type), sizeof(int));
            file.write(reinterpret_cast<char*>(&t.textureIndex), sizeof(int));
        }
    }
    file.close();
    return true;
}

bool Map::load_from_file(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        TraceLog(LOG_ERROR, "Failed to open file for writing: %s", path.c_str());
        return false;
    }

    for (int y = 0; y < WORLD_HEIGHT; ++y) {
        for (int x = 0; x < WORLD_WIDTH; ++x) {
            Tile& t = editor_map[x][y];
            if (!file.read(reinterpret_cast<char*>(&t.type), sizeof(int)))
                break; // in case old map version

            if (!file.read(reinterpret_cast<char*>(&t.textureIndex), sizeof(int))) {
                t.textureIndex = 0; // backward compatibility: default to first texture
            }
        }
    }

    file.close();
    return true;
}

void Map::draw_tilemap_previews(Editor& editor)
{
    if (textureCount == 0) {
        ImGui::Text("No tilemaps loaded.");
        return;
    }

    const float maxPreviewSize = 128.0f;
    const float spacing = 12.0f;
    const ImVec2 padding(6, 6);

    ImGui::Separator();
    ImGui::Text("Loaded Tilemaps:");

    ImGui::BeginChild("TilemapPreviewArea", ImVec2(0, maxPreviewSize + 80.0f), false, ImGuiWindowFlags_HorizontalScrollbar);
    float availWidth = ImGui::GetContentRegionAvail().x;
    float xOffset = 0.0f; // track horizontal cursor
                          //
    for (int i = 0; i < textureCount; ++i) {
        Texture2D& tex = textures[i];
        if (!tex.id)
            continue;

        // Maintain aspect ratio
        float aspect = (float)tex.width / (float)tex.height;
        float previewW, previewH;
        if (aspect >= 1.0f) {
            previewW = maxPreviewSize;
            previewH = maxPreviewSize / aspect;
        } else {
            previewH = maxPreviewSize;
            previewW = maxPreviewSize * aspect;
        }

        float cardW = maxPreviewSize + padding.x * 2;
        float cardH = maxPreviewSize + 40.0f;

        // Wrap to next line if needed
        if (xOffset + cardW > availWidth) {
            ImGui::NewLine();
            xOffset = 0.0f;
        }

        ImGui::BeginGroup();

        // Position for drawing
        ImVec2 cardPos = ImGui::GetCursorScreenPos();

        // Background
        bool isSelected = (i == editor.selectedTextureIndex);
        ImU32 bgColor = IM_COL32(50, 50, 50, 200);

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        drawList->AddRectFilled(cardPos, ImVec2(cardPos.x + cardW, cardPos.y + cardH), bgColor, 2.0f);

        // Invisible button for hover/click
        ImGui::InvisibleButton(std::to_string(i).c_str(), ImVec2(cardW, cardH));
        bool isHovered = ImGui::IsItemHovered();

        // Hover effect
        if (isHovered) {
            drawList->AddRectFilled(cardPos, ImVec2(cardPos.x + cardW, cardPos.y + cardH), IM_COL32(235, 46, 74, 100), 2.0f);
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(textureNames[i].c_str()); // full path
            ImGui::EndTooltip();
        }
        // Click to select
        if (isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            editor.selectedTextureIndex = i;

        // Draw border if selected
        if (isSelected)
            drawList->AddRect(cardPos, ImVec2(cardPos.x + cardW, cardPos.y + cardH), IM_COL32(235, 46, 74, 255), 2.0f, 0, 2.5f);
        // ImVec4() to IM_COL ---> (int)(color.x * 255.0f)

        // Draw filename
        std::string filename = std::filesystem::path(textureNames[i]).filename().string();
        float availableWidth = cardW - 4.0f; // small padding
        std::string displayName = filename;

        ImVec2 textSize = ImGui::CalcTextSize(displayName.c_str());
        if (textSize.x > availableWidth) {
            const std::string ellipsis = "...";
            int len = (int)displayName.size();
            while (len > 0 && ImGui::CalcTextSize((displayName.substr(0, len) + ellipsis).c_str()).x > availableWidth)
                len--;
            displayName = displayName.substr(0, len) + ellipsis;
        }
        ImVec2 textPos(cardPos.x + (cardW - ImGui::CalcTextSize(displayName.c_str()).x) * 0.5f, cardPos.y + 4.0f);
        drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), displayName.c_str());
        // Draw texture centered
        ImVec2 imgPos(cardPos.x + (cardW - previewW) * 0.5f, cardPos.y + 24.0f);
        drawList->AddImage((ImTextureID)(intptr_t)tex.id, imgPos, ImVec2(imgPos.x + previewW, imgPos.y + previewH));

        ImGui::Dummy(ImVec2(cardW, 10)); // Advance cursor
        ImGui::EndGroup();

        // if (i < textureCount - 1)
        // ImGui::SameLine(0, spacing);
        xOffset += cardW + spacing;
        if (xOffset < availWidth)
            ImGui::SameLine(0, spacing);
    }

    // ImGui::Dummy(ImVec2(0, 5.0f));

    ImGui::EndChild();
}

/*void Map::draw_tilemap(Editor& editor)
{
    Texture2D& tex = textures[editor.selectedTextureIndex];
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
}*/

/*void Map::draw(eZone zone)
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
 }*/
