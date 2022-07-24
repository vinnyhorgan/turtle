#include "nogame.h"

#include "raylib.h"

void noGame()
{
    SetTraceLogLevel(LOG_NONE);
    InitWindow(800, 600, "TURTLE 0.1");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    char *text = "NO GAME";
    int width = MeasureText(text, 50);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(BLACK);

        DrawText(text, 400 - width / 2, 300 - 50 / 2, 50, WHITE);

        EndDrawing();
    }

    CloseWindow();
}