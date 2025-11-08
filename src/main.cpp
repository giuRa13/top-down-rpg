#include "game.h"
#include <imgui.h>
#include <raylib.h>
#include <rlImgui.h>

int main()
{
    Game game;
    game.game_startup();

    rlImGuiSetup(true);
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    while (!WindowShouldClose()) {

        float delta = GetFrameTime();
        game.update(delta);

        BeginTextureMode(game.gameView);
        ClearBackground(GRAY);
        game.draw();
        EndTextureMode();

        BeginDrawing();
        ClearBackground(MAGENTA);

        rlImGuiBegin();
        game.draw_ui();
        rlImGuiEnd();

        EndDrawing();
    }

    UnloadRenderTexture(game.gameView);
    rlImGuiShutdown();
    CloseWindow();

    return 0;
}
