#include "game.h"
#include "raylib.h"
#include "tile.h"
#include <sys/stat.h>

Game::Game()
    : player(3, 3, eZone::WORLD)
    , chest(6, 3, eZone::ALL)
    , gate(7, 7, eZone::ALL)
    , skull(16, 9, eZone::DUNGEON)
{
    chest.health = 100;
    chest.points = 100;
    chest.is_passable = false;
    skull.is_passable = false;
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

        camera.target = {
            player.pos_x + (float)TILE_WIDTH / 2.0f,
            player.pos_y + (float)TILE_HEIGHT / 2.0f
        };
    } else if (state == eState::Editor) {

        editor_camera.target = { WORLD_WIDTH * TILE_WIDTH / 2.0f, WORLD_HEIGHT * TILE_HEIGHT / 2.0f };
    }

    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        const float zoom_increment = 0.125f;

        Camera2D* activeCam = (state == eState::Editor) ? &editor_camera : &camera;
        activeCam->zoom += (wheel * zoom_increment);

        if (activeCam->zoom < 2.0f)
            activeCam->zoom = 2.0f;
        if (activeCam->zoom > 12.0f)
            activeCam->zoom = 12.0f;
    }

    // Simple collision
    if (CheckCollisionRecs(player.hitbox, chest.hitbox)) {
        // PlaySound(sounds[SOUND_ATTACK]);
    }

    if (IsKeyPressed(KEY_E)) {
        // if (player.x_index == gate.x_index && player.y_index == gate.y_index) {
        if (CheckCollisionRecs(player.hitbox, gate.hitbox)) {
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

        map.draw_tile(chest.x_index * 16, chest.y_index * 16, 7, 1);

        map.draw_tile(gate.x_index * TILE_WIDTH, gate.y_index * TILE_HEIGHT, 3, 2);

        if (player.zone == skull.zone) {
            map.draw_tile(skull.x_index * TILE_WIDTH, skull.y_index * TILE_HEIGHT, 1, 5);
        }

        player.draw();

        if (debugMode) {

            player.draw_hitbox(RED);
            chest.draw_hitbox(BLUE);
            gate.draw_hitbox(BLUE);
            skull.draw_hitbox(BLUE);
            map.draw_grid(WORLD_WIDTH, WORLD_HEIGHT, TILE_WIDTH, TILE_HEIGHT, RED);
            draw_mouse_highlight();
        }

        EndMode2D();

    } else if (state == eState::Editor) {

        BeginMode2D(editor_camera);
        map.draw_grid(WORLD_WIDTH, WORLD_HEIGHT, TILE_WIDTH, TILE_HEIGHT, BLACK);
        draw_mouse_highlight();
        EndMode2D();
    }

    EndMode2D();

    draw_panel();
}

bool Game::can_move_to(const Rectangle& nextHitbox)
{
    // Check collision with all entities
    Entity* entities[] = { &chest, &gate, &skull };
    for (Entity* e : entities) {
        if ((e->zone == player.zone || e->zone == eZone::ALL) && !e->is_passable && CheckCollisionRecs(nextHitbox, e->hitbox)) {
            return false;
        }
    }

    // Optional: check map tiles too if needed

    return true;
}

void Game::draw_panel()
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

    ImGui::Begin("Debug Panel");
    ImGui::Text("FPS: %d", GetFPS());
    ImGui::Text("Camera: (%.2f, %.2f)", camera.target.x, camera.target.y);
    ImGui::TextUnformatted(ICON_FA_BOMB);
    ImGui::NewLine();
    map.draw_tilemap();
    ImGui::End();

    ImGui::Begin("Game View");
    ImVec2 availSize = ImGui::GetContentRegionAvail();
    float targetAspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
    // Compute width and height maintaining aspect ratio
    float width = availSize.x;
    float height = availSize.y;
    float availAspect = width / height;
    if (availAspect > targetAspect) {
        // Window is wider than target, add horizontal bars
        width = height * targetAspect;
    } else {
        // Window is taller than target, add vertical bars
        height = width / targetAspect;
    }

    // Center the viewport in ImGui window
    float offsetX = (availSize.x - width) / 2.0f;
    float offsetY = (availSize.y - height) / 2.0f;
    // Store viewport for mouse mapping ( GetWindowPos() --> top left corner )
    viewport.x = ImGui::GetWindowPos().x + offsetX;
    viewport.y = ImGui::GetWindowPos().y + offsetY;
    viewport.width = width;
    viewport.height = height;

    // Draw RenderTexture into ImGui
    ImGui::SetCursorPos(ImVec2(offsetX, offsetY));
    ImGui::Image(
        (ImTextureID)(intptr_t)gameView.texture.id,
        ImVec2(width, height),
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
