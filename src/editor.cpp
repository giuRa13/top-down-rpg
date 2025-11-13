#include "editor.h"
#include "extras/IconsFontAwesome6.h"
#include "imgui.h"
#include "map.h"
#include <ImGuiFileDialog.h>

void Editor::load_tilemap(Texture2D& tex, int tileW, int tileH)
{
    tile_width = tileW;
    tile_height = tileH;
    tiles_x = tex.width / tile_width;
    tiles_y = tex.height / tile_height;
}

void Editor::draw_tilemap_panel()
{
    if (map.textureCount == 0) {
        ImGui::Text("No tilemaps loaded");
        return;
    }

    // --- Combo box for texture selection ---
    const char* currentName = map.textureNames[selectedTextureIndex].c_str();
    if (ImGui::BeginCombo("Tilemap", currentName)) {
        for (int i = 0; i < map.textureNames.size(); ++i) {
            bool isSelected = (selectedTextureIndex == i);
            if (ImGui::Selectable(map.textureNames[i].c_str(), isSelected)) {
                selectedTextureIndex = i;

                // Update tile layout info when switching texture
                load_tilemap(map.textures[i], TILE_WIDTH, TILE_HEIGHT);
            }
            if (isSelected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    // Toggle cancel/erase mode
    ImGui::PushStyleColor(ImGuiCol_Button, cancel_tile_mode ? ImVec4(0.92f, 0.18f, 0.29f, 1.00f) : ImVec4(0.47f, 0.77f, 0.83f, 0.14f));
    if (ImGui::Button(std::string(ICON_FA_ERASER " Cancel").c_str())) {
        cancel_tile_mode = !cancel_tile_mode;
        if (cancel_tile_mode)
            fill_all_mode = false;
    }
    ImGui::PopStyleColor();
    ImGui::SameLine();

    // Toggle fill all tiles mode
    ImGui::PushStyleColor(ImGuiCol_Button, fill_all_mode ? ImVec4(0.92f, 0.18f, 0.29f, 1.00f) : ImVec4(0.47f, 0.77f, 0.83f, 0.14f));
    if (ImGui::Button(std::string(ICON_FA_CHESS_BOARD " Fill").c_str())) {
        fill_all_mode = !fill_all_mode;
        if (fill_all_mode)
            cancel_tile_mode = false;
    }
    ImGui::PopStyleColor();

    // --- Draw current tilemap grid ---
    Texture2D& tex = map.textures[selectedTextureIndex];
    if (!tex.id) {
        ImGui::Text("Invalid texture");
        return;
    }

    // just for conflict with tilemaps previes
    int new_tiles_x = tex.width / tile_width;
    int new_tiles_y = tex.height / tile_height;
    if (new_tiles_x != tiles_x || new_tiles_y != tiles_y) {
        load_tilemap(tex, tile_width, tile_height);
    }

    for (int y = 0; y < tiles_y; ++y) {
        for (int x = 0; x < tiles_x; ++x) {
            ImVec2 uv0 = {
                (float)(x * tile_width) / tex.width,
                (float)(y * tile_height) / tex.height
            };
            ImVec2 uv1 = {
                (float)((x + 1) * tile_width) / tex.width,
                (float)((y + 1) * tile_height) / tex.height
            };

            ImVec2 button_size(tile_width * 2, tile_height * 2);

            bool selected = (selected_index_x == x && selected_index_y == y);
            if (selected)
                ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 255, 0, 255));

            std::string id = "tile_" + std::to_string(x) + "_" + std::to_string(y);
            if (ImGui::ImageButton(id.c_str(),
                    (ImTextureID)(intptr_t)tex.id,
                    button_size, uv0, uv1)) {
                selected_index_x = x;
                selected_index_y = y;
            }

            if (selected)
                ImGui::PopStyleColor();

            ImGui::SameLine();
        }
        ImGui::NewLine();
    }
}

Rectangle Editor::get_selected_tile_rect() const
{
    return Rectangle {
        (float)(selected_index_x * tile_width),
        (float)(selected_index_y * tile_height),
        (float)tile_width,
        (float)tile_height
    };
}

void Editor::draw_editor_bar()
{
    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("New"))
            reset_map();

        if (ImGui::MenuItem("Save")) {
            if (!currentFilePath.empty())
                map.save_to_file(currentFilePath);
            else
                saveDialogOpen = true; // trigger Save As dialog
        }

        if (ImGui::MenuItem("Save As..."))
            saveDialogOpen = true;

        if (ImGui::MenuItem("Load"))
            loadDialogOpen = true;

        if (ImGui::MenuItem("Exit"))
            CloseWindow();

        ImGui::EndMenu();
    }

    // --- Save As Dialog ---
    if (saveDialogOpen) {
        if (!ImGuiFileDialog::Instance()->IsOpened("SaveAsDlgKey"))
            ImGuiFileDialog::Instance()->OpenDialog("SaveAsDlgKey", "Save Map As", ".bin\0.*\0");

        if (ImGuiFileDialog::Instance()->Display("SaveAsDlgKey")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                currentFilePath = ImGuiFileDialog::Instance()->GetFilePathName();
                map.save_to_file(currentFilePath);
            }
            ImGuiFileDialog::Instance()->Close();
            saveDialogOpen = false;
        }
    }

    // --- Load Dialog ---
    if (loadDialogOpen) {
        if (!ImGuiFileDialog::Instance()->IsOpened("LoadDlgKey"))
            ImGuiFileDialog::Instance()->OpenDialog("LoadDlgKey", "Load Map", ".bin\0.*\0");

        if (ImGuiFileDialog::Instance()->Display("LoadDlgKey")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                currentFilePath = ImGuiFileDialog::Instance()->GetFilePathName();
                map.load_from_file(currentFilePath);
            }
            ImGuiFileDialog::Instance()->Close();
            loadDialogOpen = false;
        }
    }
}

void Editor::reset_map()
{
    for (int x = 0; x < WORLD_WIDTH; ++x)
        for (int y = 0; y < WORLD_HEIGHT; ++y)
            map.editor_map[x][y].type = -1;

    currentFilePath.clear();
}

void Editor::open_save_as_dialog()
{
    if (!saveDialogOpen) {
        // Only call OpenDialog once when triggered
        ImGuiFileDialog::Instance()->OpenDialog("SaveAsDlgKey", "Save Map As", ".bin\0.*\0");
        saveDialogOpen = true;
    }

    // Display the dialog
    if (ImGuiFileDialog::Instance()->Display("SaveAsDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            currentFilePath = filePathName;
            map.save_to_file(currentFilePath);
        }
        ImGuiFileDialog::Instance()->Close();
        saveDialogOpen = false;
    }
}

void Editor::open_load_dialog()
{
    if (!loadDialogOpen) {
        ImGuiFileDialog::Instance()->OpenDialog("LoadDlgKey", "Load Map", ".bin\0.*\0");
        loadDialogOpen = true;
    }

    if (ImGuiFileDialog::Instance()->Display("LoadDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            currentFilePath = filePathName;
            map.load_from_file(currentFilePath);
        }
        ImGuiFileDialog::Instance()->Close();
        loadDialogOpen = false;
    }
}
