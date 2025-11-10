#include "game.h"
#include "imgui.h"
#include "raylib.h"
#include "tile.h"
#include <cstddef>
#include <sys/stat.h>

Game::Game()
    : player(3, 3, eZone::WORLD)
{
    spawn_entity<Entity>("chest", 6, 3, eZone::ALL);
    entity_map["chest"]->is_passable = false;
    entity_map["chest"]->health = 100;
    entity_map["chest"]->points = 100;

    spawn_entity<Entity>("gate", 7, 7, eZone::ALL);

    spawn_entity<Entity>("skull", 18, 7, eZone::DUNGEON);
    entity_map["skull"]->is_passable = false;

    spawn_entity<Entity>("explosion_f", 9, 2, eZone::ALL);
    entity_map["explosion_f"]->hasAnimation = true;

    spawn_entity<Entity>("explosion_d", 11, 10, eZone::ALL, 2.0f);
    entity_map["explosion_d"]->hasAnimation = true;

    spawn_entity<Entity>("trap1", 12, 5, eZone::WORLD);
    entity_map["trap1"]->hasAnimation = true;

    spawn_entity<Entity>("trap2", 13, 5, eZone::WORLD);
    entity_map["trap2"]->hasAnimation = true;

    spawn_entity<Entity>("trap3", 14, 5, eZone::WORLD);
    entity_map["trap3"]->hasAnimation = true;

    init_camera();
}

void Game::game_startup()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "RPG");
    SetTargetFPS(60);
    InitAudioDevice();
    gameView = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);

    map.init();
    player.load();

    Image explosion_fImage = LoadImage(RESOURCES_PATH "explosion_1f.png");
    Texture2D explosion_fTex = LoadTextureFromImage(explosion_fImage);
    auto explosion_f = get_entity("explosion_f");
    explosion_f->baseAnim.init(explosion_fTex, 1, 8, 48, 0.15f, 1, true);
    UnloadImage(explosion_fImage);

    Image explosion_dImage = LoadImage(RESOURCES_PATH "explosion_1d.png");
    Texture2D explosion_dTex = LoadTextureFromImage(explosion_dImage);
    auto explosion_d = get_entity("explosion_d");
    explosion_d->baseAnim.init(explosion_dTex, 1, 12, 128, 0.15f, 1, true);
    UnloadImage(explosion_dImage);

    Image trapImage = LoadImage(RESOURCES_PATH "trap.png");
    Texture2D trapTex = LoadTextureFromImage(trapImage);
    auto trap_1 = get_entity("trap1");
    auto trap_2 = get_entity("trap2");
    auto trap_3 = get_entity("trap3");
    trap_1->baseAnim.init(trapTex, 1, 8, 16, 0.15f, 1, true);
    trap_2->baseAnim.init(trapTex, 1, 8, 16, 0.15f, 1, true);
    trap_3->baseAnim.init(trapTex, 1, 8, 16, 0.15f, 1, true);
    UnloadImage(trapImage);

    sounds[SOUND_ATTACK] = LoadSound(RESOURCES_PATH "human_damage_3.wav");
    sounds[SOUND_POINTS] = LoadSound(RESOURCES_PATH "win_sound.wav");
}

void Game::init_camera()
{
    camera = { 0 };
    camera.zoom = 3.0f;
    camera.target = { player.pos_x, player.pos_y };
    camera.offset = { 600, 300 };

    editor_camera = { 0 };
    editor_camera.zoom = 2.0f;
    editor_camera.target = { WORLD_WIDTH * TILE_WIDTH / 2.0f, WORLD_HEIGHT * TILE_HEIGHT / 2.0f };
    editor_camera.offset = { viewportWidth / 2.0f, viewportHeight / 2.0f };
}

void Game::update(float delta)
{
    if (IsKeyPressed(KEY_TAB)) {
        if (state == eState::Game) {
            state = eState::Editor;
        } else {
            state = eState::Game;
        }
    }

    if (IsKeyPressed(KEY_R))
        debugMode = !debugMode;

    if (state == eState::Game) {

        player.update(delta, *this);

        for (auto& e : entities)
            e->update(delta);

        camera.target = {
            player.pos_x + (float)TILE_WIDTH / 2.0f,
            player.pos_y + (float)TILE_HEIGHT / 2.0f
        };
    } else if (state == eState::Editor) {

        editor_camera.target = { WORLD_WIDTH * TILE_WIDTH / 2.0f, WORLD_HEIGHT * TILE_HEIGHT / 2.0f };
    }

    float wheel = GetMouseWheelMove();
    ImVec2 mousePos = ImGui::GetMousePos();
    bool mouseOverGame = (mousePos.x >= viewport.x && mousePos.x <= viewport.x + viewport.width && mousePos.y >= viewport.y && mousePos.y <= viewport.y + viewport.height);

    if (wheel != 0 && mouseOverGame) {
        const float zoom_increment = 0.125f;

        Camera2D* activeCam = (state == eState::Editor) ? &editor_camera : &camera;
        activeCam->zoom += (wheel * zoom_increment);

        if (activeCam->zoom < 2.0f)
            activeCam->zoom = 2.0f;
        if (activeCam->zoom > 12.0f)
            activeCam->zoom = 12.0f;
    }

    // Simple collision
    if (CheckCollisionRecs(player.hitbox, get_entity("chest")->hitbox)) {
        // PlaySound(sounds[SOUND_ATTACK]);
    }

    if (IsKeyPressed(KEY_E)) {
        // if (player.x_index == gate.x_index && player.y_index == gate.y_index) {
        if (CheckCollisionRecs(player.hitbox, get_entity("gate")->hitbox)) {
            if (player.zone == eZone::WORLD)
                player.zone = eZone::DUNGEON;

            else if (player.zone == eZone::DUNGEON)
                player.zone = eZone::WORLD;

            PlaySound(sounds[SOUND_POINTS]);
        }
    }
}

void Game::draw()
{
    if (state == eState::Game) {

        BeginMode2D(camera);

        map.draw(player.zone);

        auto chest = get_entity("chest");
        auto gate = get_entity("gate");
        auto skull = get_entity("skull");
        map.draw_tile(chest->x_index * 16, chest->y_index * 16, 7, 1);
        map.draw_tile(gate->x_index * TILE_WIDTH, gate->y_index * TILE_HEIGHT, 3, 2);

        if (player.zone == skull->zone) {
            map.draw_tile(skull->x_index * TILE_WIDTH, skull->y_index * TILE_HEIGHT, 1, 5);
        }

        for (size_t i = 3; i < entities.size(); ++i) {
            entities[i]->draw();
        }

        player.draw();

        if (debugMode) {
            player.draw_hitbox(RED);

            /*for (auto& e : entities) {
                e->show_hitbox = true;
            }*/

            map.draw_grid(WORLD_WIDTH, WORLD_HEIGHT, TILE_WIDTH, TILE_HEIGHT, RED);
            draw_mouse_highlight();
        }

        for (auto& e : entities) {
            if (e->show_hitbox)
                e->draw_hitbox(BLUE);
        }

        EndMode2D();

    } else if (state == eState::Editor) {

        BeginMode2D(editor_camera);
        map.draw_grid(WORLD_WIDTH, WORLD_HEIGHT, TILE_WIDTH, TILE_HEIGHT, BLACK);
        draw_mouse_highlight();
        EndMode2D();
    }

    EndMode2D();

    draw_overlay();
}

bool Game::can_move_to(const Rectangle& nextHitbox)
{
    for (auto& e : entities) {
        if ((e->zone == player.zone || e->zone == eZone::ALL)
            && !e->is_passable
            && e->is_alive
            && CheckCollisionRecs(nextHitbox, e->hitbox)) {
            return false;
        }
    }

    // Optional: check map tiles too if needed

    return true;
}

void Game::draw_entity_panel()
{
    ImGui::Begin("Entities");

    for (auto& e : entities) {
        ImGui::PushID(e->name.c_str()); // Unique ID for each entity
        // --- Set blue background for headers ---
        // ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.4f, 0.8f, 0.8f)); // darker blue
        // ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.3f, 0.5f, 1.0f, 0.9f)); // brighter hover
        // ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.1f, 0.3f, 0.7f, 1.0f)); // pressed blue

        // Create a collapsible tree node for each entity
        bool open = ImGui::CollapsingHeader(
            e->name.c_str() //,
            // ImGuiTreeNodeFlags_DefaultOpen // remove this if you want them collapsed by default
        );

        // ImGui::PopStyleColor(3);

        if (open) {
            ImGui::Indent();

            const char* zoneNames[] = { "WORLD", "DUNGEON", "ALL" };
            int zoneIndex = static_cast<int>(e->zone);
            if (ImGui::Combo("Zone", &zoneIndex, zoneNames, IM_ARRAYSIZE(zoneNames))) {
                e->zone = static_cast<eZone>(zoneIndex);
            }
            // ImGui::Text("Zone: %d", static_cast<int>(e->zone));
            ImGui::Text("Alive: %s", e->is_alive ? "Yes" : "No");
            ImGui::Text("Passable: %s", e->is_passable ? "Yes" : "No");
            ImGui::Text("Health: %d", e->health);
            ImGui::Text("Damage: %d", e->damage);
            ImGui::Text("Points: %d", e->points);
            ImGui::Text("Pos: (%d, %d)", e->x_index, e->y_index);
            ImGui::SliderInt("Pos X", &e->x_index, 0, WORLD_WIDTH);
            ImGui::SliderInt("Pos Y##", &e->y_index, 0, WORLD_HEIGHT);
            e->update_hitbox();
            ImGui::Text("Hitbox: x=%.1f y=%.1f w=%.1f h=%.1f",
                e->hitbox.x, e->hitbox.y, e->hitbox.width, e->hitbox.height);

            if (e->hasAnimation) {
                ImGui::Text("Animation Preview:");
                const float maxPreviewSize = 256.0f; // all previews fit in this square
                float texW = static_cast<float>(e->baseAnim.texture.width);
                float texH = static_cast<float>(e->baseAnim.texture.height);

                // Keep aspect ratio
                float aspect = texW / texH;
                float drawW, drawH;
                if (aspect >= 1.0f) {
                    drawW = maxPreviewSize;
                    drawH = maxPreviewSize / aspect;
                } else {
                    drawH = maxPreviewSize;
                    drawW = maxPreviewSize * aspect;
                }

                ImVec2 size(drawW, drawH);

                // ImGui expects ImTextureID; with raylib, we cast the texture id
                ImGui::Image(
                    (ImTextureID)(intptr_t)e->baseAnim.texture.id,
                    size,
                    ImVec2(0, 0),
                    ImVec2(1, 1));
            } else {
                ImGui::TextDisabled("No animation texture.");
            }

            ImGui::Checkbox("hitbox", &e->show_hitbox);

            ImGui::Unindent();
            ImGui::Separator();
        }
        ImGui::PopID();
    }

    ImGui::End();
}

void Game::draw_overlay()
{
    DrawRectangle(5, 5, 330, 150, Fade(BLACK, 0.5f));
    DrawRectangleLines(5, 5, 330, 150, RED);

    DrawText(TextFormat("FPS: %6.2f", 1.0f / GetFrameTime()), 15, 15, 20, RED);
    DrawText(TextFormat("Camera Target: (%06.2f, %06.2f)", camera.target.x, camera.target.y), 15, 35, 14, RAYWHITE);
    DrawText(TextFormat("Camera Zoom: %06.2f", camera.zoom), 15, 50, 14, RAYWHITE);

    DrawText(TextFormat("Tile: %d - %d", player.x_index, player.y_index), 15, 75, 14, ORANGE);
    DrawText(TextFormat("Pos: %4.2f - %4.2f", player.pos_x, player.pos_y), 15, 90, 14, ORANGE);

    DrawText(TextFormat("Player Health: %d", player.health), 15, 105, 14, ORANGE);
    DrawText(TextFormat("Player Points: %d", player.points), 15, 120, 14, ORANGE);
}

void Game::draw_ui()
{
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    // ---- Debug Panel ----
    ImGui::Begin("Debug Panel");
    ImGui::Text("Camera: (%.2f, %.2f)", camera.target.x, camera.target.y);
    ImGui::TextUnformatted(ICON_FA_BOMB);
    ImGui::NewLine();
    map.draw_tilemap();
    ImGui::End();

    // ---- Game View ----
    ImGui::Begin("Game View");

    // Get the size of the ImGui content area
    ImVec2 availSize = ImGui::GetContentRegionAvail();

    // Desired aspect ratio of the game (usually your screen resolution)
    const float targetAspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
    const float windowAspect = availSize.x / availSize.y;

    float drawWidth = availSize.x;
    float drawHeight = availSize.y;

    // Adjust to preserve aspect ratio
    if (windowAspect > targetAspect) {
        // Window is wider → add side black bars
        drawWidth = availSize.y * targetAspect;
    } else {
        // Window is taller → add top/bottom black bars
        drawHeight = availSize.x / targetAspect;
    }

    // Center the image in the ImGui window
    float offsetX = ((availSize.x - drawWidth) * 0.5f) + 5;
    float offsetY = ((availSize.y - drawHeight) * 0.5f) + 30;

    // Store viewport rectangle for mouse world conversion
    viewport.x = ImGui::GetWindowPos().x + offsetX;
    viewport.y = ImGui::GetWindowPos().y + offsetY;
    viewport.width = drawWidth;
    viewport.height = drawHeight;

    // Set cursor to where the image should begin
    ImGui::SetCursorPos(ImVec2(offsetX, offsetY));

    // Draw the RenderTexture (with correct UV flip)
    ImGui::Image(
        (ImTextureID)(intptr_t)gameView.texture.id,
        ImVec2(drawWidth, drawHeight),
        ImVec2(0, 1),
        ImVec2(1, 0));

    ImGui::End();
}

void Game::draw_mouse_highlight()
{
    /*Vector2 mouseScreen = GetMousePosition();
    Vector2 mouseWorld = GetScreenToWorld2D(mouseScreen, camera);

    int tileX = (int)(mouseWorld.x / TILE_WIDTH);
    int tileY = (int)(mouseWorld.y / TILE_HEIGHT);

    DrawRectangle(tileX * TILE_WIDTH, tileY * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, Fade(GREEN, 0.4f));*/

    Camera2D& cam = (state == eState::Editor) ? editor_camera : camera;

    ImVec2 mousePos = ImGui::GetMousePos();
    Vector2 mouseScreen = { mousePos.x, mousePos.y };
    float mouseX = mouseScreen.x - viewport.x;
    float mouseY = mouseScreen.y - viewport.y;

    // Skip if outside viewport
    if (mouseX < 0 || mouseY < 0 || mouseX > viewport.width || mouseY > viewport.height)
        return;

    float scaleX = (float)SCREEN_WIDTH / viewport.width;
    float scaleY = (float)SCREEN_HEIGHT / viewport.height;
    Vector2 mouseWorld = GetScreenToWorld2D({ mouseX * scaleX, mouseY * scaleY }, cam);

    int tileX = (int)(mouseWorld.x / TILE_WIDTH);
    int tileY = (int)(mouseWorld.y / TILE_HEIGHT);
    DrawRectangle(tileX * TILE_WIDTH, tileY * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, Fade(GREEN, 0.4f));
}
