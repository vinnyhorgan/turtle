#include "duktape.h"
#include "duk_v1_compat.h"
#include "duk_module_duktape.h"
#include "duk_console.h"

#include "raylib.h"

#include "chipmunk/chipmunk.h"

#include "map.h"
#include "uuid4.h"

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_WINDOW_WIDTH 800
#define INITIAL_WINDOW_HEIGHT 600
#define VERSION 0.1

State state;

duk_ret_t modSearch(duk_context *ctx)
{
    const char *id = duk_get_string(ctx, 0);

    char fullname[100] = {0};

    strcat(fullname, id);
    strcat(fullname, ".js");

    printf("Loading module: %s\n", fullname);

    duk_push_string_file(ctx, fullname);

    return 1;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        noGame();
        return 0;
    }

    duk_context *ctx = duk_create_heap_default();

    if (!ctx)
    {
        printf("Error creating javascript context.\n");
        return 1;
    }

    state.close = false;
    state.title = "TURTLE 0.1";
    state.vSync = true;
    state.grabbed = false;
    state.currentColor = WHITE;
    state.currentBackgroundColor = BLACK;
    state.currentFont = GetFontDefault();
    state.space = cpSpaceNew();

    cpVect gravity = cpv(0, 500);
    cpSpaceSetGravity(state.space, gravity);

    map_init(&state.keys);
    map_init(&state.images);
    map_init(&state.fonts);
    map_init(&state.sounds);
    map_init(&state.colliders);
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

    char *buffer = malloc(strlen(argv[1]) + 9);
    strcpy(buffer, argv[1]);
    strcat(buffer, "/main.js");

    SetTraceLogLevel(LOG_NONE);
    InitWindow(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, state.title);
    SetExitKey(KEY_NULL);

    InitAudioDevice();

    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

    if (duk_peval_file(ctx, buffer) != 0)
        printf("%s\n", duk_safe_to_string(ctx, -1));

    while (!WindowShouldClose() && !state.close)
    {
        cpSpaceStep(state.space, GetFrameTime());

        duk_get_global_string(ctx, "update");
        duk_push_number(ctx, GetFrameTime());

        if (duk_pcall(ctx, 1) != 0)
            printf("%s\n", duk_safe_to_string(ctx, -1));

        duk_pop(ctx);

        BeginDrawing();

        ClearBackground(state.currentBackgroundColor);

        duk_get_global_string(ctx, "draw");

        if (duk_pcall(ctx, 0) != 0)
            printf("%s\n", duk_safe_to_string(ctx, -1));

        duk_pop(ctx);

        EndDrawing();

        vec_clear(&state.collisions);
    }

    CloseWindow();

    CloseAudioDevice();

    free(buffer);

    // TRY TO CLEANUP IMAGES, FONTS AND SOUNDS AND PHYSICS!!! LOADS OF MEMORY LEAKS...

    map_deinit(&state.keys);
    map_deinit(&state.images);
    map_deinit(&state.fonts);
    map_deinit(&state.sounds);
    map_deinit(&state.colliders);
    vec_deinit(&state.collisions);

    duk_destroy_heap(ctx);

    return 0;
}