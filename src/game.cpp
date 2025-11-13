#include "game.h"
#include "editor.h"
#include "imgui.h"
#include "raylib.h"
#include "tile.h"

Game::Game()
    : player(3, 3, eZone::WORLD)
    , editor(map)
{
    auto chest = entity_registry.spawn<Entity>("chest", 6, 3, eZone::ALL);
    chest->is_passable = false;
    chest->health = 100;
    chest->points = 100;

    entity_registry.spawn<Entity>("gate", 7, 7, eZone::ALL);

    auto skull = entity_registry.spawn<Entity>("skull", 18, 7, eZone::DUNGEON);
    skull->is_passable = false;

    auto explosion_f = entity_registry.spawn<Entity>("explosion_f", 9, 2, eZone::ALL);
    explosion_f->hasAnimation = true;

    auto explosion_d = entity_registry.spawn<Entity>("explosion_d", 11, 10, eZone::ALL, 2.0f);
    explosion_d->hasAnimation = true;

    auto trap1 = entity_registry.spawn<Entity>("trap1", 12, 5, eZone::WORLD);
    trap1->hasAnimation = true;

    auto trap2 = entity_registry.spawn<Entity>("trap2", 13, 5, eZone::WORLD);
    trap2->hasAnimation = true;

    auto trap3 = entity_registry.spawn<Entity>("trap3", 14, 5, eZone::WORLD);
    trap3->hasAnimation = true;

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
    init_editor();

    Image explosion_fImage = LoadImage(RESOURCES_PATH "explosion_1f.png");
    Texture2D explosion_fTex = LoadTextureFromImage(explosion_fImage);
    auto explosion_f = entity_registry.get("explosion_f");
    explosion_f->baseAnim.init(explosion_fTex, 1, 8, 48, 0.15f, 1, true);
    UnloadImage(explosion_fImage);

    Image explosion_dImage = LoadImage(RESOURCES_PATH "explosion_1d.png");
    Texture2D explosion_dTex = LoadTextureFromImage(explosion_dImage);
    auto explosion_d = entity_registry.get("explosion_d");
    explosion_d->baseAnim.init(explosion_dTex, 1, 12, 128, 0.15f, 1, true);
    UnloadImage(explosion_dImage);

    Image trapImage = LoadImage(RESOURCES_PATH "trap.png");
    Texture2D trapTex = LoadTextureFromImage(trapImage);
    auto trap_1 = entity_registry.get("trap1");
    auto trap_2 = entity_registry.get("trap2");
    auto trap_3 = entity_registry.get("trap3");
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

void Game::init_editor()
{
    if (map.textureCount > 0) {
        editor.selectedTextureIndex = 0;
        editor.load_tilemap(map.textures[0], 16, 16);
    }
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

    if (IsKeyPressed(KEY_F))
        free_cam = !free_cam;

    if (state == eState::Game) {

        player.update(delta, *this);

        for (auto& e : entity_registry.get_all())
            e->update(delta);

        if (free_cam) {
            float cameraSpeed = 200.0f * delta;
            if (IsKeyDown(KEY_LEFT))
                camera.target.x -= cameraSpeed;
            if (IsKeyDown(KEY_RIGHT))
                camera.target.x += cameraSpeed;
            if (IsKeyDown(KEY_UP))
                camera.target.y -= cameraSpeed;
            if (IsKeyDown(KEY_DOWN))
                camera.target.y += cameraSpeed;
        } else {
            camera.target = {
                player.pos_x + (float)TILE_WIDTH / 2.0f,
                player.pos_y + (float)TILE_HEIGHT / 2.0f
            };
        }
    } else if (state == eState::Editor) {

        if (free_cam) {
            float cameraSpeed = 200.0f * delta;
            if (IsKeyDown(KEY_LEFT))
                editor_camera.target.x -= cameraSpeed;
            if (IsKeyDown(KEY_RIGHT))
                editor_camera.target.x += cameraSpeed;
            if (IsKeyDown(KEY_UP))
                editor_camera.target.y -= cameraSpeed;
            if (IsKeyDown(KEY_DOWN))
                editor_camera.target.y += cameraSpeed;
        } else {
            editor_camera.target = { WORLD_WIDTH * TILE_WIDTH / 2.0f, WORLD_HEIGHT * TILE_HEIGHT / 2.0f };
        }
    }

    float wheel = GetMouseWheelMove();
    ImVec2 mousePos = ImGui::GetMousePos();
    bool mouseOverGame = (mousePos.x >= viewport.x && mousePos.x <= viewport.x + viewport.width && mousePos.y >= viewport.y && mousePos.y <= viewport.y + viewport.height);

    Camera2D* activeCam = (state == eState::Editor) ? &editor_camera : &camera;
    if (wheel != 0 && mouseOverGame) {
        const float zoom_increment = 0.125f;

        // Camera2D* activeCam = (state == eState::Editor) ? &editor_camera : &camera;
        activeCam->zoom += (wheel * zoom_increment);

        if (activeCam->zoom < 2.0f)
            activeCam->zoom = 2.0f;
        if (activeCam->zoom > 12.0f)
            activeCam->zoom = 12.0f;
    }

    // Simple collision
    if (CheckCollisionRecs(player.hitbox, entity_registry.get("chest")->hitbox)) {
        // PlaySound(sounds[SOUND_ATTACK]);
    }

    if (IsKeyPressed(KEY_E)) {
        // if (player.x_index == gate.x_index && player.y_index == gate.y_index) {
        if (CheckCollisionRecs(player.hitbox, entity_registry.get("gate")->hitbox)) {
            if (player.zone == eZone::WORLD)
                player.zone = eZone::DUNGEON;

            else if (player.zone == eZone::DUNGEON)
                player.zone = eZone::WORLD;

            PlaySound(sounds[SOUND_POINTS]);
        }
    }

    handle_entity_selection();
}

void Game::draw()
{
    if (state == eState::Game) {

        BeginMode2D(camera);

        map.draw_grid(WORLD_WIDTH, WORLD_HEIGHT, TILE_WIDTH, TILE_HEIGHT, 1.0f, BLACK);
        map.draw();

        for (auto& e : entity_registry.get_all()) {
            // Only draw if visible in current zone
            if (e->zone == eZone::ALL || e->zone == player.zone) {
                if (e->hasAnimation) {
                    // Animated entities use their own draw()
                    e->draw();
                } else {
                    // Static entities use map tiles
                    if (e->name == "chest")
                        map.draw_tile(e->x_index * 16, e->y_index * 16, 7, 1, "dungeon_test");
                    else if (e->name == "gate")
                        map.draw_tile(e->x_index * TILE_WIDTH, e->y_index * TILE_HEIGHT, 2, 2, "dungeon_test");
                    else if (e->name == "skull")
                        map.draw_tile(e->x_index * TILE_WIDTH, e->y_index * TILE_HEIGHT, 1, 5, "dungeon_test");
                }
            }
        }

        player.draw();

        if (debugMode) {
            player.draw_hitbox(RED);

            for (auto& e : entity_registry.get_all()) {
                e->show_hitbox = true;
                e->draw_hitbox(BLUE);
            }

            map.draw_grid(WORLD_WIDTH, WORLD_HEIGHT, TILE_WIDTH, TILE_HEIGHT, 0.5f, RED);
            draw_mouse_highlight();
        }

        if (selected_entity) {
            DrawRectangleLinesEx(selected_entity->hitbox, 1, YELLOW);
        }

        EndMode2D();

    } else if (state == eState::Editor) {

        BeginMode2D(editor_camera);
        EditorViewport view { viewport.x, viewport.y, viewport.width, viewport.height };
        map.draw_editor_map(view, editor, editor_camera);
        // draw_mouse_highlight();
        EndMode2D();
    }

    draw_overlay();
}

bool Game::can_move_to(const Rectangle& nextHitbox)
{
    for (auto& e : entity_registry.get_all()) {
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

void Game::handle_entity_selection()
{
    const auto& io = ImGui::GetIO();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        ImVec2 mousePos = ImGui::GetMousePos();

        // Translate to game-view local coordinates
        float localX = mousePos.x - viewport.x;
        float localY = mousePos.y - viewport.y;

        // Skip if outside viewport bounds
        if (localX >= 0 && localY >= 0 && localX <= viewport.width && localY <= viewport.height) {
            // Convert from ImGui window space to your world space
            float scaleX = (float)SCREEN_WIDTH / viewport.width;
            float scaleY = (float)SCREEN_HEIGHT / viewport.height;

            Vector2 mouseWorld = GetScreenToWorld2D(
                { localX * scaleX, localY * scaleY },
                (state == eState::Editor) ? editor_camera : camera);

            // Detect clicked entity
            selected_entity = nullptr;
            for (auto& e : entity_registry.get_all()) {
                if (CheckCollisionPointRec(mouseWorld, e->hitbox)) {
                    selected_entity = e.get();
                    break;
                }
            }
        }
    }
}

void Game::draw_entity_panel()
{
    // ImGui::Begin("Entities");

    for (auto& e : entity_registry.get_all()) {
        ImGui::PushID(e->name.c_str()); // Unique ID for each entity
        // --- Set blue background for headers ---
        // ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.4f, 0.8f, 0.8f)); // darker blue
        // ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.3f, 0.5f, 1.0f, 0.9f)); // brighter hover
        // ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.1f, 0.3f, 0.7f, 1.0f)); // pressed blue

        /*bool open = ImGui::CollapsingHeader(
            e->name.c_str() //, ImGuiTreeNodeFlags_DefaultOpen
        );*/
        bool open = false;
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;
        if (selected_entity == e.get())
            open = ImGui::CollapsingHeader(e->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
        else
            open = ImGui::CollapsingHeader(e->name.c_str());

        if (ImGui::IsItemToggledOpen())
            selected_entity = e.get();

        // ImGui::PopStyleColor(3);

        if (open) {
            ImGui::Indent();

            const char* zoneNames[] = { "ALL", "WORLD", "DUNGEON" };
            int zoneIndex = static_cast<int>(e->zone);
            if (ImGui::Combo("Zone", &zoneIndex, zoneNames, IM_ARRAYSIZE(zoneNames))) {
                e->zone = static_cast<eZone>(zoneIndex);
            }
            // ImGui::Text("Zone: %d", static_cast<int>(e->zone));

            ImGui::Checkbox("Alive", &e->is_alive);
            ImGui::Checkbox("Passable", &e->is_passable);

            ImGui::InputInt("Health", &e->health);
            e->health = std::max(0, e->health);
            ImGui::InputInt("Damage", &e->damage);
            e->damage = std::max(0, e->damage);
            ImGui::InputInt("Points", &e->points);
            e->points = std::max(0, e->points);

            // ImGui::NewLine();
            ImGui::Separator();
            ImGui::Text("Pos: (%d, %d)", e->x_index, e->y_index);
            ImGui::Text("Hitbox: x=%.1f y=%.1f w=%.1f h=%.1f",
                e->hitbox.x, e->hitbox.y, e->hitbox.width, e->hitbox.height);
            ImGui::SliderInt("Pos X", &e->x_index, 0, WORLD_WIDTH);
            ImGui::SliderInt("Pos Y##", &e->y_index, 0, WORLD_HEIGHT);
            e->update_hitbox();

            // ImGui::NewLine();
            ImGui::Separator();
            ImGui::Text("Animation Preview:");
            if (e->hasAnimation) {
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

                // Reserve space and get the rectangle area where the image will be drawn
                ImVec2 p0 = ImGui::GetCursorScreenPos();
                ImVec2 p1 = ImVec2(p0.x + size.x, p0.y + size.y);

                ImGui::GetWindowDrawList()->AddRectFilled(
                    p0, p1, /*IM_COL32(255, 0, 255, 255)*/ IM_COL32(0, 0, 0, 255));

                // ImGui expects ImTextureID; with raylib, we cast the texture id
                ImGui::Image(
                    (ImTextureID)(intptr_t)e->baseAnim.texture.id,
                    size,
                    ImVec2(0, 0),
                    ImVec2(1, 1));
            } else {
                ImGui::TextDisabled("No animation texture.");
            }

            ImGui::Unindent();
            ImGui::Separator();
        }
        ImGui::PopID();
    }

    // ImGui::End();
}

void Game::draw_ui()
{
    ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    if (ImGui::BeginMainMenuBar()) {

        editor.draw_editor_bar();

        if (ImGui::BeginMenu("Mode")) {
            if (ImGui::MenuItem("Game", nullptr, state == eState::Game))
                state = eState::Game;

            if (ImGui::MenuItem("Debug", nullptr, debugMode))
                debugMode = !debugMode;

            if (ImGui::MenuItem("Editor", nullptr, state == eState::Editor))
                state = eState::Editor;

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Camera")) {
            if (ImGui::MenuItem("Game", nullptr, !free_cam))
                if (free_cam)
                    free_cam = false;

            if (ImGui::MenuItem("Free Camera", nullptr, free_cam))
                free_cam = !free_cam;

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    // ---- Debug Panel ----
    ImGui::Begin("Debug Panel");
    ImGui::Text("Camera: (%.2f, %.2f)", camera.target.x, camera.target.y);
    ImGui::TextUnformatted(ICON_FA_BOMB);
    ImGui::NewLine();
    map.draw_tilemap_previews(editor);
    ImGui::End();

    ImGui::Begin("panel");
    if (state == eState::Game)
        draw_entity_panel();
    else if (state == eState::Editor)
        editor.draw_tilemap_panel();
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

void Game::draw_overlay()
{
    DrawRectangle(5, 5, 330, 220, Fade(BLACK, 0.5f));
    DrawRectangleLines(5, 5, 330, 220, RED);

    DrawText(TextFormat("FPS: %6.2f", 1.0f / GetFrameTime()), 15, 15, 20, RED);
    if (state == eState::Game) {
        DrawText(TextFormat("Camera Target: %.2f - %.2f", camera.target.x, camera.target.y), 15, 35, 14, RAYWHITE);
        DrawText(TextFormat("Camera Zoom: %06.2f", camera.zoom), 15, 50, 14, RAYWHITE);

        DrawText(TextFormat("Tile: %d - %d", player.x_index, player.y_index), 15, 75, 14, ORANGE);
        DrawText(TextFormat("Pos: %4.2f - %4.2f", player.pos_x, player.pos_y), 15, 90, 14, ORANGE);

        DrawText(TextFormat("Player Health: %d", player.health), 15, 105, 14, ORANGE);
        DrawText(TextFormat("Player Points: %d", player.points), 15, 120, 14, ORANGE);

        if (!debugMode)
            DrawText("Game Mode", 15, 150, 20, RED);
        else
            DrawText("Debug Mode", 15, 150, 20, RED);
    } else {

        DrawText("Editor Mode", 15, 35, 20, RED);
    }

    if (free_cam) {
        if (state == eState::Game)
            DrawText("Free Camera", 15, 170, 20, RED);
        else
            DrawText("Free Camera", 15, 55, 20, RED);
    }
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

    DrawRectangle(tileX * TILE_WIDTH, tileY * TILE_HEIGHT, TILE_WIDTH, TILE_HEIGHT, Fade(GREEN, 0.2f));
}
