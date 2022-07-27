#include "duktape.h"
#include "duk_v1_compat.h"
#include "duk_module_duktape.h"
#include "duk_console.h"

#include "raylib.h"

#include "chipmunk/chipmunk.h"
#include "enet/enet.h"

#include "map.h"
#include "uuid4.h"

#include "sds.h"

#include "state.h"
#include "nogame.h"

#include "graphics.h"
#include "keyboard.h"
#include "mouse.h"
#include "system.h"
#include "timer.h"
#include "window.h"
#include "audio.h"
#include "filesystem.h"
#include "math.h"
#include "physics.h"
#include "camera.h"
#include "network.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define INITIAL_WINDOW_WIDTH 800
#define INITIAL_WINDOW_HEIGHT 600
#define VERSION 0.1

State state;

duk_ret_t modSearch(duk_context *ctx)
{
    const char *id = duk_get_string(ctx, 0);

    sds filename = sdsnew(state.baseDir);

    filename = sdscat(filename, "/");
    filename = sdscat(filename, id);
    filename = sdscat(filename, ".js");

    printf("Loading module: %s\n", filename);

    duk_push_string_file(ctx, filename);

    sdsfree(filename);

    return 1;
}

void error(duk_context *ctx)
{
    duk_get_prop_string(ctx, -1, "stack");
    strcpy(state.errorString, duk_safe_to_string(ctx, -1));
    state.error = true;
}

void sigintHandler(int sig)
{
    state.close = true;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        noGame();
        return 0;
    }

    signal(SIGINT, sigintHandler);

    duk_context *ctx = duk_create_heap_default();

    if (!ctx)
    {
        printf("Error creating javascript context.\n");
        return 1;
    }

    state.close = false;
    state.error = false;
    state.title = "TURTLE 0.1";
    state.vSync = true;
    state.grabbed = false;
    state.currentColor = WHITE;
    state.currentBackgroundColor = BLACK;
    state.currentFont = GetFontDefault();
    state.space = cpSpaceNew();
    state.typescript = false;
    state.baseDir = argv[1];

    state.camera.target = (Vector2){0, 0};
    state.camera.zoom = 1.0f;

    char command[100];
    strcpy(command, "rm ");
    strcat(command, state.baseDir);
    strcat(command, "/*.js ");

    cpVect gravity = cpv(0, 500);
    cpSpaceSetGravity(state.space, gravity);

    enet_initialize();

    map_init(&state.keys);
    map_init(&state.images);
    map_init(&state.fonts);
    map_init(&state.sounds);
    map_init(&state.colliders);
    map_init(&state.hosts);
    map_init(&state.peers);

    vec_init(&state.collisions);

    uuid4_init();

    duk_console_init(ctx, DUK_CONSOLE_PROXY_WRAPPER);
    duk_module_duktape_init(ctx);

    duk_get_global_string(ctx, "Duktape");
    duk_push_c_function(ctx, modSearch, 4);
    duk_put_prop_string(ctx, -2, "modSearch");
    duk_pop(ctx);

    duk_push_object(ctx);
    duk_put_global_string(ctx, "turtle");

    registerGraphicsFunctions(ctx);
    registerKeyboardFunctions(ctx);
    registerMouseFunctions(ctx);
    registerSystemFunctions(ctx);
    registerTimerFunctions(ctx);
    registerWindowFunctions(ctx);
    registerAudioFunctions(ctx);
    registerMathFunctions(ctx);
    registerFilesystemFunctions(ctx);
    registerPhysicsFunctions(ctx);
    registerCameraFunctions(ctx);
    registerNetworkFunctions(ctx);

    SetTraceLogLevel(LOG_NONE);
    InitWindow(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, state.title);
    SetExitKey(KEY_NULL);

    InitAudioDevice();

    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

    char mainJs[100];
    strcpy(mainJs, state.baseDir);
    strcat(mainJs, "/main.js");

    char mainTs[100];
    strcpy(mainTs, state.baseDir);
    strcat(mainTs, "/main.ts");

    if (FileExists(mainJs))
    {
        if (duk_peval_file(ctx, mainJs) != DUK_EXEC_SUCCESS)
            error(ctx);
    }
    else if(FileExists(mainTs))
    {
        char command[100];
        strcpy(command, "swc ");
        strcat(command, state.baseDir);
        strcat(command, " -d ");
        strcat(command, state.baseDir);
        strcat(command, " -q");

        if (system(command) != 0)
        {
            strcpy(state.errorString, "Error compiling Typescript, you might need to install swc.");
            state.error = true;
        }
        else
        {
            if (duk_peval_file(ctx, mainJs) != DUK_EXEC_SUCCESS)
                error(ctx);
        }

        state.typescript = true;
    }
    else
    {
        strcpy(state.errorString, "Invalid argument, point to a directory containing a main.js or main.ts file.");
        state.error = true;
    }

    while (!WindowShouldClose() && !state.close)
    {
        if (!state.error)
        {
            cpSpaceStep(state.space, GetFrameTime());

            duk_get_global_string(ctx, "update");
            duk_push_number(ctx, GetFrameTime());

            if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS)
                error(ctx);

            duk_pop(ctx);

            BeginDrawing();

            ClearBackground(state.currentBackgroundColor);

            duk_get_global_string(ctx, "draw");

            if (duk_pcall(ctx, 0) != DUK_EXEC_SUCCESS)
                error(ctx);

            duk_pop(ctx);

            EndDrawing();

            vec_clear(&state.collisions);
        }
        else
        {
            static bool copied = false;

            if (IsMouseButtonPressed(0))
            {
                SetClipboardText(state.errorString);
                copied = true;
            }

            BeginDrawing();

            ClearBackground(SKYBLUE);

            DrawText(state.errorString, 50, 100, 20, WHITE);
            DrawText("Click to copy message", 50, 400, 20, WHITE);

            if (copied)
            {
                DrawText("Copied!", 50, 450, 20, WHITE);
            }

            EndDrawing();
        }
    }

    CloseWindow();

    CloseAudioDevice();

    // TRY TO CLEANUP IMAGES, FONTS AND SOUNDS AND PHYSICS!!! LOADS OF MEMORY LEAKS...

    map_deinit(&state.keys);
    map_deinit(&state.images);
    map_deinit(&state.fonts);
    map_deinit(&state.sounds);
    map_deinit(&state.colliders);
    map_deinit(&state.hosts);
    map_init(&state.peers);

    vec_deinit(&state.collisions);

    duk_destroy_heap(ctx);

    enet_deinitialize();

    if (state.typescript)
    {
        char command[100];
        strcpy(command, "rm ");
        strcat(command, state.baseDir);
        strcat(command, "/*.js ");

        if (system(command) != 0)
            printf("Error removing compiled JavaScript files.");
    }

    return 0;
}