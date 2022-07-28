#include "chipmunk/chipmunk.h"
#include "enet/enet.h"
#include "raylib.h"

#include "duktape.h"
#include "duk_v1_compat.h"
#include "duk_module_duktape.h"
#include "duk_console.h"

#include "sds.h"
#include "map.h"
#include "vec.h"
#include "uuid4.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define VERSION "alpha 0.1"

// STRUCTS

typedef struct Collider
{
    cpBody *body;
    cpShape *shape;
    const char *class;
} Collider;

typedef struct Collision
{
    const char *idA;
    const char *idB;
} Collision;

typedef struct Client
{
    const char *id;
    const char *address;
    int port;
} Client;

// STATE

typedef map_t(Texture2D) img_map_t;
typedef map_t(Font) fnt_map_t;
typedef map_t(Sound) snd_map_t;
typedef map_t(Collider) col_map_t;
typedef map_t(ENetHost *) host_map_t;
typedef map_t(ENetPeer *) peer_map_t;

typedef vec_t(Collision) col_vec_t;

typedef struct State
{
    bool close;
    bool error;
    char errorString[1000];
    char *title;
    bool vSync;
    bool grabbed;
    bool typescript;
    const char *baseDir;
    map_int_t keys;
    Color currentColor;
    Color currentBackgroundColor;
    Font currentFont;
    img_map_t images;
    fnt_map_t fonts;
    snd_map_t sounds;
    cpSpace *space;
    col_map_t colliders;
    Camera2D camera;
    col_vec_t collisions;
    host_map_t hosts;
    peer_map_t peers;
} State;

State state;

// AUDIO MODULE

duk_ret_t audioNewSource(duk_context *ctx)
{
    const char *filename = duk_require_string(ctx, 0);

    sds path = sdsempty();
    path = sdscatprintf(path, "%s/%s", state.baseDir, filename);

    if (!FileExists(path))
    {
        duk_push_error_object(ctx, DUK_ERR_ERROR, "File does not exist.");
        duk_throw(ctx);
    }

    Sound sound = LoadSound(path);

    sdsfree(path);

    char soundId[UUID4_LEN];
    uuid4_generate(soundId);
    map_set(&state.sounds, soundId, sound);

    duk_push_string(ctx, soundId);

    return 1;
}

// REFACTORED

duk_ret_t audioSetMasterVolume(duk_context *ctx)
{
    float volume = duk_require_number(ctx, 0);

    SetMasterVolume(volume);

    return 0;
}

duk_ret_t audioPlay(duk_context *ctx)
{
    const char *soundId = duk_require_string(ctx, 0);

    Sound sound = *map_get(&state.sounds, soundId);

    PlaySound(sound);

    return 0;
}

duk_ret_t audioStop(duk_context *ctx)
{
    const char *soundId = duk_require_string(ctx, 0);

    Sound sound = *map_get(&state.sounds, soundId);

    StopSound(sound);

    return 0;
}

duk_ret_t audioPause(duk_context *ctx)
{
    const char *soundId = duk_require_string(ctx, 0);

    Sound sound = *map_get(&state.sounds, soundId);

    PauseSound(sound);

    return 0;
}

duk_ret_t audioResume(duk_context *ctx)
{
    const char *soundId = duk_require_string(ctx, 0);

    Sound sound = *map_get(&state.sounds, soundId);

    ResumeSound(sound);

    return 0;
}

duk_ret_t audioIsPlaying(duk_context *ctx)
{
    const char *soundId = duk_require_string(ctx, 0);

    Sound sound = *map_get(&state.sounds, soundId);

    bool playing = IsSoundPlaying(sound);

    duk_push_boolean(ctx, playing);

    return 1;
}

duk_ret_t audioSetVolume(duk_context *ctx)
{
    const char *soundId = duk_require_string(ctx, 0);
    float volume = duk_require_number(ctx, 1);

    Sound sound = *map_get(&state.sounds, soundId);

    SetSoundVolume(sound, volume);

    return 0;
}

duk_ret_t audioSetPitch(duk_context *ctx)
{
    const char *soundId = duk_require_string(ctx, 0);
    float pitch = duk_require_number(ctx, 1);

    Sound sound = *map_get(&state.sounds, soundId);

    SetSoundPitch(sound, pitch);

    return 0;
}

void registerAudioFunctions(duk_context *ctx)
{
    duk_get_global_string(ctx, "turtle");
    duk_push_object(ctx);
    duk_put_prop_string(ctx, -2, "audio");

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "audio");
    duk_push_c_function(ctx, audioNewSource, 1);
    duk_put_prop_string(ctx, -2, "newSource");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "audio");
    duk_push_c_function(ctx, audioSetMasterVolume, 1);
    duk_put_prop_string(ctx, -2, "setMasterVolume");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "audio");
    duk_push_c_function(ctx, audioPlay, 1);
    duk_put_prop_string(ctx, -2, "play");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "audio");
    duk_push_c_function(ctx, audioStop, 1);
    duk_put_prop_string(ctx, -2, "stop");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "audio");
    duk_push_c_function(ctx, audioPause, 1);
    duk_put_prop_string(ctx, -2, "pause");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "audio");
    duk_push_c_function(ctx, audioResume, 1);
    duk_put_prop_string(ctx, -2, "resume");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "audio");
    duk_push_c_function(ctx, audioIsPlaying, 1);
    duk_put_prop_string(ctx, -2, "isPlaying");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "audio");
    duk_push_c_function(ctx, audioSetVolume, 2);
    duk_put_prop_string(ctx, -2, "setVolume");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "audio");
    duk_push_c_function(ctx, audioSetPitch, 2);
    duk_put_prop_string(ctx, -2, "setPitch");
    duk_pop(ctx);
}

// CAMERA MODULE

duk_ret_t cameraAttach(duk_context *ctx)
{
    BeginMode2D(state.camera);

    return 0;
}

duk_ret_t cameraDetach(duk_context *ctx)
{
    EndMode2D();

    return 0;
}

duk_ret_t cameraLookAt(duk_context *ctx)
{
    int x = duk_require_number(ctx, 0);
    int y = duk_require_number(ctx, 1);

    state.camera.target = (Vector2){x, y};

    return 0;
}

duk_ret_t cameraZoom(duk_context *ctx)
{
    float zoom = duk_require_number(ctx, 0);

    state.camera.zoom = zoom;

    return 0;
}

duk_ret_t cameraRotate(duk_context *ctx)
{
    float rotation = duk_require_number(ctx, 0);

    state.camera.rotation = rotation;

    return 0;
}

duk_ret_t cameraToWorldX(duk_context *ctx)
{
    int x = duk_require_number(ctx, 0);

    int transformed = x + state.camera.target.x;

    duk_push_number(ctx, transformed);

    return 1;
}

duk_ret_t cameraToWorldY(duk_context *ctx)
{
    int y = duk_require_number(ctx, 0);

    int transformed = y + state.camera.target.y;

    duk_push_number(ctx, transformed);

    return 1;
}

duk_ret_t cameraGetX(duk_context *ctx)
{
    int x = state.camera.target.x;

    duk_push_number(ctx, x);

    return 1;
}

duk_ret_t cameraGetY(duk_context *ctx)
{
    int y = state.camera.target.y;

    duk_push_number(ctx, y);

    return 1;
}

duk_ret_t cameraGetZoom(duk_context *ctx)
{
    float zoom = state.camera.zoom;

    duk_push_number(ctx, zoom);

    return 1;
}

duk_ret_t cameraGetRotation(duk_context *ctx)
{
    float rotation = state.camera.rotation;

    duk_push_number(ctx, rotation);

    return 1;
}

void registerCameraFunctions(duk_context *ctx)
{
    duk_get_global_string(ctx, "turtle");
    duk_push_object(ctx);
    duk_put_prop_string(ctx, -2, "camera");

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "camera");
    duk_push_c_function(ctx, cameraAttach, 0);
    duk_put_prop_string(ctx, -2, "attach");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "camera");
    duk_push_c_function(ctx, cameraDetach, 0);
    duk_put_prop_string(ctx, -2, "detach");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "camera");
    duk_push_c_function(ctx, cameraLookAt, 2);
    duk_put_prop_string(ctx, -2, "lookAt");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "camera");
    duk_push_c_function(ctx, cameraZoom, 1);
    duk_put_prop_string(ctx, -2, "zoom");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "camera");
    duk_push_c_function(ctx, cameraRotate, 1);
    duk_put_prop_string(ctx, -2, "rotate");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "camera");
    duk_push_c_function(ctx, cameraToWorldX, 1);
    duk_put_prop_string(ctx, -2, "toWorldX");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "camera");
    duk_push_c_function(ctx, cameraToWorldY, 1);
    duk_put_prop_string(ctx, -2, "toWorldY");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "camera");
    duk_push_c_function(ctx, cameraGetX, 0);
    duk_put_prop_string(ctx, -2, "getX");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "camera");
    duk_push_c_function(ctx, cameraGetY, 0);
    duk_put_prop_string(ctx, -2, "getY");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "camera");
    duk_push_c_function(ctx, cameraGetZoom, 0);
    duk_put_prop_string(ctx, -2, "getZoom");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "camera");
    duk_push_c_function(ctx, cameraGetRotation, 0);
    duk_put_prop_string(ctx, -2, "getRotation");
    duk_pop(ctx);
}

void registerFilesystemFunctions(duk_context *ctx)
{
    duk_get_global_string(ctx, "turtle");
    duk_push_object(ctx);
    duk_put_prop_string(ctx, -2, "filesystem");
}

duk_ret_t graphicsCircle(duk_context *ctx)
{
    const char *mode = duk_require_string(ctx, 0);
    int x = duk_require_number(ctx, 1);
    int y = duk_require_number(ctx, 2);
    float radius = duk_require_number(ctx, 3);

    if (strcmp("line", mode) == 0)
    {
        DrawCircleLines(x, y, radius, state.currentColor);
    }
    else if (strcmp("fill", mode) == 0)
    {
        DrawCircle(x, y, radius, state.currentColor);
    }

    return 0;
}

duk_ret_t graphicsDraw(duk_context *ctx)
{
    const char *imageId = duk_require_string(ctx, 0);
    int x = duk_require_number(ctx, 1);
    int y = duk_require_number(ctx, 2);
    float rotation = duk_require_number(ctx, 3);
    float scale = duk_require_number(ctx, 4);

    Texture2D image = *map_get(&state.images, imageId);

    DrawTextureEx(image, (Vector2){x, y}, rotation, scale, state.currentColor);

    return 0;
}

duk_ret_t graphicsEllipse(duk_context *ctx)
{
    const char *mode = duk_require_string(ctx, 0);
    int x = duk_require_number(ctx, 1);
    int y = duk_require_number(ctx, 2);
    float radiusX = duk_require_number(ctx, 3);
    float radiusY = duk_require_number(ctx, 4);

    if (strcmp("line", mode) == 0)
    {
        DrawEllipseLines(x, y, radiusX, radiusY, state.currentColor);
    }
    else if (strcmp("fill", mode) == 0)
    {
        DrawEllipse(x, y, radiusX, radiusY, state.currentColor);
    }

    return 0;
}

duk_ret_t graphicsLine(duk_context *ctx)
{
    int x1 = duk_require_number(ctx, 0);
    int y1 = duk_require_number(ctx, 1);
    int x2 = duk_require_number(ctx, 2);
    int y2 = duk_require_number(ctx, 3);

    DrawLine(x1, y1, x2, y2, state.currentColor);

    return 0;
}

duk_ret_t graphicsPoint(duk_context *ctx)
{
    int x = duk_require_number(ctx, 0);
    int y = duk_require_number(ctx, 1);

    DrawPixel(x, y, state.currentColor);

    return 0;
}

duk_ret_t graphicsPrint(duk_context *ctx)
{
    const char *text = duk_require_string(ctx, 0);
    int x = duk_require_number(ctx, 1);
    int y = duk_require_number(ctx, 2);
    int size = duk_require_number(ctx, 3);

    DrawTextEx(state.currentFont, text, (Vector2){x, y}, size, 2, state.currentColor);

    return 0;
}

duk_ret_t graphicsRectangle(duk_context *ctx)
{
    const char *mode = duk_require_string(ctx, 0);
    int x = duk_require_number(ctx, 1);
    int y = duk_require_number(ctx, 2);
    int width = duk_require_number(ctx, 3);
    int height = duk_require_number(ctx, 4);

    if (strcmp("line", mode) == 0)
    {
        DrawRectangleLines(x, y, width, height, state.currentColor);
    }
    else if (strcmp("fill", mode) == 0)
    {
        DrawRectangle(x, y, width, height, state.currentColor);
    }

    return 0;
}

duk_ret_t graphicsTriangle(duk_context *ctx)
{
    const char *mode = duk_require_string(ctx, 0);
    int x1 = duk_require_number(ctx, 1);
    int y1 = duk_require_number(ctx, 2);
    int x2 = duk_require_number(ctx, 3);
    int y2 = duk_require_number(ctx, 4);
    int x3 = duk_require_number(ctx, 5);
    int y3 = duk_require_number(ctx, 6);

    if (strcmp("line", mode) == 0)
    {
        DrawTriangleLines((Vector2){x1, y1}, (Vector2){x2, y2}, (Vector2){x3, y3}, state.currentColor);
    }
    else if (strcmp("fill", mode) == 0)
    {
        DrawTriangle((Vector2){x1, y1}, (Vector2){x2, y2}, (Vector2){x3, y3}, state.currentColor);
    }

    return 0;
}

duk_ret_t graphicsNewImage(duk_context *ctx)
{
    const char *filename = duk_require_string(ctx, 0);

    char path[1000];
    strcpy(path, state.baseDir);
    strcat(path, "/");
    strcat(path, filename);

    Texture2D image = LoadTexture(path);

    char imageId[UUID4_LEN];
    uuid4_generate(imageId);

    map_set(&state.images, imageId, image);

    duk_push_string(ctx, imageId);

    return 1;
}

duk_ret_t graphicsNewFont(duk_context *ctx)
{
    const char *filename = duk_require_string(ctx, 0);

    Font font = LoadFont(filename);

    char fontId[UUID4_LEN];
    uuid4_generate(fontId);

    map_set(&state.fonts, fontId, font);

    duk_push_string(ctx, fontId);

    return 1;
}

duk_ret_t graphicsCaptureScreenshot(duk_context *ctx)
{
    const char *filename = duk_require_string(ctx, 0);

    TakeScreenshot(filename);

    return 0;
}

duk_ret_t graphicsSetBackgroundColor(duk_context *ctx)
{
    int r = duk_require_number(ctx, 0);
    int g = duk_require_number(ctx, 1);
    int b = duk_require_number(ctx, 2);
    int a = duk_require_number(ctx, 3);

    Color color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;

    state.currentBackgroundColor = color;

    return 0;
}

duk_ret_t graphicsSetColor(duk_context *ctx)
{
    int r = duk_require_number(ctx, 0);
    int g = duk_require_number(ctx, 1);
    int b = duk_require_number(ctx, 2);
    int a = duk_require_number(ctx, 3);

    Color color;
    color.r = r;
    color.g = g;
    color.b = b;
    color.a = a;

    state.currentColor = color;

    return 0;
}

duk_ret_t graphicsSetFont(duk_context *ctx)
{
    const char *fontId = duk_require_string(ctx, 0);

    Font font = *map_get(&state.fonts, fontId);

    state.currentFont = font;

    return 0;
}

void registerGraphicsFunctions(duk_context *ctx)
{
    duk_get_global_string(ctx, "turtle");
    duk_push_object(ctx);
    duk_put_prop_string(ctx, -2, "graphics");

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "graphics");
    duk_push_c_function(ctx, graphicsCircle, 4);
    duk_put_prop_string(ctx, -2, "circle");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "graphics");
    duk_push_c_function(ctx, graphicsDraw, 5);
    duk_put_prop_string(ctx, -2, "draw");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "graphics");
    duk_push_c_function(ctx, graphicsEllipse, 5);
    duk_put_prop_string(ctx, -2, "ellipse");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "graphics");
    duk_push_c_function(ctx, graphicsLine, 4);
    duk_put_prop_string(ctx, -2, "line");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "graphics");
    duk_push_c_function(ctx, graphicsPoint, 2);
    duk_put_prop_string(ctx, -2, "point");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "graphics");
    duk_push_c_function(ctx, graphicsPrint, 4);
    duk_put_prop_string(ctx, -2, "print");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "graphics");
    duk_push_c_function(ctx, graphicsRectangle, 5);
    duk_put_prop_string(ctx, -2, "rectangle");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "graphics");
    duk_push_c_function(ctx, graphicsTriangle, 7);
    duk_put_prop_string(ctx, -2, "triangle");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "graphics");
    duk_push_c_function(ctx, graphicsNewImage, 1);
    duk_put_prop_string(ctx, -2, "newImage");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "graphics");
    duk_push_c_function(ctx, graphicsNewFont, 1);
    duk_put_prop_string(ctx, -2, "newFont");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "graphics");
    duk_push_c_function(ctx, graphicsCaptureScreenshot, 1);
    duk_put_prop_string(ctx, -2, "captureScreenshot");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "graphics");
    duk_push_c_function(ctx, graphicsSetBackgroundColor, 4);
    duk_put_prop_string(ctx, -2, "setBackgroundColor");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "graphics");
    duk_push_c_function(ctx, graphicsSetColor, 4);
    duk_put_prop_string(ctx, -2, "setColor");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "graphics");
    duk_push_c_function(ctx, graphicsSetFont, 1);
    duk_put_prop_string(ctx, -2, "setFont");
    duk_pop(ctx);
}

duk_ret_t keyboardIsDown(duk_context *ctx)
{
    const char *key = duk_require_string(ctx, 0);

    bool down = IsKeyDown(*map_get(&state.keys, key));

    duk_push_boolean(ctx, down);

    return 1;
}

duk_ret_t keyboardIsPressed(duk_context *ctx)
{
    const char *key = duk_require_string(ctx, 0);

    bool pressed = IsKeyPressed(*map_get(&state.keys, key));

    duk_push_boolean(ctx, pressed);

    return 1;
}

duk_ret_t keyboardIsReleased(duk_context *ctx)
{
    const char *key = duk_require_string(ctx, 0);

    bool released = IsKeyReleased(*map_get(&state.keys, key));

    duk_push_boolean(ctx, released);

    return 1;
}

void registerKeyboardFunctions(duk_context *ctx)
{
    map_set(&state.keys, "a", KEY_A);
    map_set(&state.keys, "b", KEY_B);
    map_set(&state.keys, "c", KEY_C);
    map_set(&state.keys, "d", KEY_D);
    map_set(&state.keys, "e", KEY_E);
    map_set(&state.keys, "f", KEY_F);
    map_set(&state.keys, "g", KEY_G);
    map_set(&state.keys, "h", KEY_H);
    map_set(&state.keys, "i", KEY_I);
    map_set(&state.keys, "j", KEY_J);
    map_set(&state.keys, "k", KEY_K);
    map_set(&state.keys, "l", KEY_L);
    map_set(&state.keys, "m", KEY_M);
    map_set(&state.keys, "n", KEY_N);
    map_set(&state.keys, "o", KEY_O);
    map_set(&state.keys, "p", KEY_P);
    map_set(&state.keys, "q", KEY_Q);
    map_set(&state.keys, "r", KEY_R);
    map_set(&state.keys, "s", KEY_S);
    map_set(&state.keys, "t", KEY_T);
    map_set(&state.keys, "u", KEY_U);
    map_set(&state.keys, "v", KEY_V);
    map_set(&state.keys, "w", KEY_W);
    map_set(&state.keys, "x", KEY_X);
    map_set(&state.keys, "y", KEY_Y);
    map_set(&state.keys, "z", KEY_Z);
    map_set(&state.keys, "0", KEY_ZERO);
    map_set(&state.keys, "1", KEY_ONE);
    map_set(&state.keys, "2", KEY_TWO);
    map_set(&state.keys, "3", KEY_THREE);
    map_set(&state.keys, "4", KEY_FOUR);
    map_set(&state.keys, "5", KEY_FIVE);
    map_set(&state.keys, "6", KEY_SIX);
    map_set(&state.keys, "7", KEY_SEVEN);
    map_set(&state.keys, "8", KEY_EIGHT);
    map_set(&state.keys, "9", KEY_NINE);
    map_set(&state.keys, "space", KEY_SPACE);
    map_set(&state.keys, "up", KEY_UP);
    map_set(&state.keys, "down", KEY_DOWN);
    map_set(&state.keys, "left", KEY_LEFT);
    map_set(&state.keys, "right", KEY_RIGHT);
    map_set(&state.keys, "home", KEY_HOME);
    map_set(&state.keys, "end", KEY_END);
    map_set(&state.keys, "pageup", KEY_PAGE_UP);
    map_set(&state.keys, "pagedown", KEY_PAGE_DOWN);
    map_set(&state.keys, "insert", KEY_INSERT);
    map_set(&state.keys, "backspace", KEY_BACKSPACE);
    map_set(&state.keys, "tab", KEY_TAB);
    map_set(&state.keys, "return", KEY_ENTER);
    map_set(&state.keys, "delete", KEY_DELETE);
    map_set(&state.keys, "f1", KEY_F1);
    map_set(&state.keys, "f2", KEY_F2);
    map_set(&state.keys, "f3", KEY_F3);
    map_set(&state.keys, "f4", KEY_F4);
    map_set(&state.keys, "f5", KEY_F5);
    map_set(&state.keys, "f6", KEY_F6);
    map_set(&state.keys, "f7", KEY_F7);
    map_set(&state.keys, "f8", KEY_F8);
    map_set(&state.keys, "f9", KEY_F9);
    map_set(&state.keys, "f10", KEY_F10);
    map_set(&state.keys, "f11", KEY_F11);
    map_set(&state.keys, "f12", KEY_F12);
    map_set(&state.keys, "numlock", KEY_NUM_LOCK);
    map_set(&state.keys, "capslock", KEY_CAPS_LOCK);
    map_set(&state.keys, "scolllock", KEY_SCROLL_LOCK);
    map_set(&state.keys, "rshift", KEY_RIGHT_SHIFT);
    map_set(&state.keys, "lshift", KEY_LEFT_SHIFT);
    map_set(&state.keys, "rctrl", KEY_RIGHT_CONTROL);
    map_set(&state.keys, "lctrl", KEY_LEFT_CONTROL);
    map_set(&state.keys, "ralt", KEY_RIGHT_ALT);
    map_set(&state.keys, "lalt", KEY_LEFT_ALT);
    map_set(&state.keys, "lsuper", KEY_LEFT_SUPER);
    map_set(&state.keys, "rsuper", KEY_RIGHT_SUPER);
    map_set(&state.keys, "escape", KEY_ESCAPE);

    duk_get_global_string(ctx, "turtle");
    duk_push_object(ctx);
    duk_put_prop_string(ctx, -2, "keyboard");

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "keyboard");
    duk_push_c_function(ctx, keyboardIsDown, 1);
    duk_put_prop_string(ctx, -2, "isDown");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "keyboard");
    duk_push_c_function(ctx, keyboardIsPressed, 1);
    duk_put_prop_string(ctx, -2, "isPressed");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "keyboard");
    duk_push_c_function(ctx, keyboardIsReleased, 1);
    duk_put_prop_string(ctx, -2, "isReleased");
    duk_pop(ctx);
}

duk_ret_t mathRandom(duk_context *ctx)
{
    int min = duk_require_number(ctx, 0);
    int max = duk_require_number(ctx, 1);

    int random = GetRandomValue(min, max);

    duk_push_number(ctx, random);

    return 1;
}

duk_ret_t mathSetRandomSeed(duk_context *ctx)
{
    int seed = duk_require_number(ctx, 0);

    SetRandomSeed(seed);

    return 0;
}

void registerMathFunctions(duk_context *ctx)
{
    duk_get_global_string(ctx, "turtle");
    duk_push_object(ctx);
    duk_put_prop_string(ctx, -2, "math");

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "math");
    duk_push_c_function(ctx, mathRandom, 2);
    duk_put_prop_string(ctx, -2, "random");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "math");
    duk_push_c_function(ctx, mathSetRandomSeed, 1);
    duk_put_prop_string(ctx, -2, "setRandomSeed");
    duk_pop(ctx);
}

duk_ret_t mouseGetX(duk_context *ctx)
{
    int x = GetMouseX();

    duk_push_number(ctx, x);

    return 1;
}

duk_ret_t mouseGetY(duk_context *ctx)
{
    int y = GetMouseY();

    duk_push_number(ctx, y);

    return 1;
}

duk_ret_t mouseIsDown(duk_context *ctx)
{
    int button = duk_require_number(ctx, 0);

    bool down = IsMouseButtonDown(button);

    duk_push_boolean(ctx, down);

    return 1;
}

duk_ret_t mouseIsPressed(duk_context *ctx)
{
    int button = duk_require_number(ctx, 0);

    bool pressed = IsMouseButtonPressed(button);

    duk_push_boolean(ctx, pressed);

    return 1;
}

duk_ret_t mouseIsReleased(duk_context *ctx)
{
    int button = duk_require_number(ctx, 0);

    bool released = IsMouseButtonReleased(button);

    duk_push_boolean(ctx, released);

    return 1;
}

duk_ret_t mouseGetWheelMove(duk_context *ctx)
{
    float wheelMove = GetMouseWheelMove();

    duk_push_number(ctx, wheelMove);

    return 1;
}

duk_ret_t mouseSetGrabbed(duk_context *ctx)
{
    bool grab = duk_require_boolean(ctx, 0);

    if (grab)
    {
        DisableCursor();
        state.grabbed = true;
    }
    else
    {
        EnableCursor();
        state.grabbed = false;
    }

    return 0;
}

duk_ret_t mouseIsGrabbed(duk_context *ctx)
{
    bool grabbed = state.grabbed;

    duk_push_boolean(ctx, grabbed);

    return 1;
}

duk_ret_t mouseSetVisible(duk_context *ctx)
{
    bool visible = duk_require_boolean(ctx, 0);

    if (visible)
    {
        ShowCursor();
    }
    else
    {
        HideCursor();
    }

    return 0;
}

duk_ret_t mouseIsVisible(duk_context *ctx)
{
    bool visible = !IsCursorHidden();

    duk_push_boolean(ctx, visible);

    return 1;
}

void registerMouseFunctions(duk_context *ctx)
{
    duk_get_global_string(ctx, "turtle");
    duk_push_object(ctx);
    duk_put_prop_string(ctx, -2, "mouse");

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "mouse");
    duk_push_c_function(ctx, mouseGetX, 0);
    duk_put_prop_string(ctx, -2, "getX");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "mouse");
    duk_push_c_function(ctx, mouseGetY, 0);
    duk_put_prop_string(ctx, -2, "getY");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "mouse");
    duk_push_c_function(ctx, mouseIsDown, 1);
    duk_put_prop_string(ctx, -2, "isDown");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "mouse");
    duk_push_c_function(ctx, mouseIsPressed, 1);
    duk_put_prop_string(ctx, -2, "isPressed");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "mouse");
    duk_push_c_function(ctx, mouseIsReleased, 1);
    duk_put_prop_string(ctx, -2, "isReleased");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "mouse");
    duk_push_c_function(ctx, mouseGetWheelMove, 0);
    duk_put_prop_string(ctx, -2, "getWheelMove");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "mouse");
    duk_push_c_function(ctx, mouseSetGrabbed, 1);
    duk_put_prop_string(ctx, -2, "setGrabbed");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "mouse");
    duk_push_c_function(ctx, mouseIsGrabbed, 0);
    duk_put_prop_string(ctx, -2, "isGrabbed");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "mouse");
    duk_push_c_function(ctx, mouseSetVisible, 1);
    duk_put_prop_string(ctx, -2, "setVisible");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "mouse");
    duk_push_c_function(ctx, mouseIsVisible, 0);
    duk_put_prop_string(ctx, -2, "isVisible");
    duk_pop(ctx);
}

duk_ret_t networkNewServer(duk_context *ctx)
{
    const char *address = duk_require_string(ctx, 0);
    int port = duk_require_number(ctx, 1);

    ENetAddress enetAddress;
    enet_address_set_host(&enetAddress, address);
    enetAddress.port = port;

    ENetHost *server = enet_host_create(&enetAddress, 32, 2, 0, 0);

    if (server == NULL)
    {
        duk_push_error_object(ctx, DUK_ERR_ERROR, "Could not create server.");
        duk_throw(ctx);
    }

    char hostId[UUID4_LEN];
    uuid4_generate(hostId);

    map_set(&state.hosts, hostId, server);

    duk_push_string(ctx, hostId);

    return 1;
}

duk_ret_t networkNewClient(duk_context *ctx)
{
    ENetHost *client = enet_host_create(NULL, 1, 2, 0, 0);

    if (client == NULL)
    {
        duk_push_error_object(ctx, DUK_ERR_ERROR, "Could not create client.");
        duk_throw(ctx);
    }

    char hostId[UUID4_LEN];
    uuid4_generate(hostId);

    map_set(&state.hosts, hostId, client);

    duk_push_string(ctx, hostId);

    return 1;
}

duk_ret_t networkService(duk_context *ctx)
{
    const char *host = duk_require_string(ctx, 0);
    int timeout = duk_require_number(ctx, 1);

    ENetEvent event;

    ENetHost *enetHost = *map_get(&state.hosts, host);

    enet_host_service(enetHost, &event, timeout);

    sds type = sdsempty();

    switch (event.type)
    {
    case ENET_EVENT_TYPE_CONNECT:
        sdscat(type, "connect");
        break;
    case ENET_EVENT_TYPE_DISCONNECT:
        sdscat(type, "disconnect");
        break;
    case ENET_EVENT_TYPE_RECEIVE:
        sdscat(type, "receive");
        break;
    case ENET_EVENT_TYPE_NONE:
        break;
    }

    if (event.type != 0)
    {
        char peerId[UUID4_LEN];
        uuid4_generate(peerId);

        map_set(&state.peers, peerId, event.peer);

        duk_idx_t obj = duk_push_object(ctx);
        duk_push_string(ctx, type);
        duk_put_prop_string(ctx, obj, "type");
        duk_push_string(ctx, peerId);
        duk_put_prop_string(ctx, obj, "peer");

        if (strcmp(type, "receive") == 0)
        {
            duk_push_string(ctx, (char *)event.packet->data);
            duk_put_prop_string(ctx, obj, "data");
        }
    }
    else
    {
        duk_idx_t obj = duk_push_object(ctx);
        duk_push_string(ctx, "none");
        duk_put_prop_string(ctx, obj, "type");
    }

    return 1;
}

duk_ret_t networkSend(duk_context *ctx)
{
    const char *peerId = duk_require_string(ctx, 0);
    const char *data = duk_require_string(ctx, 1);
    const char *method = duk_get_string(ctx, 2);

    if (method == NULL)
    {
        method = "reliable";
    }

    ENetPacketFlag enetMethod;

    if (strcmp(data, "reliable") == 0)
    {
        enetMethod = ENET_PACKET_FLAG_RELIABLE;
    }
    else if (strcmp(data, "unreliable") == 0)
    {
        enetMethod = ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT;
    }
    else
    {
        enetMethod = ENET_PACKET_FLAG_RELIABLE;
    }

    ENetPeer *peer = *map_get(&state.peers, peerId);

    ENetPacket *packet = enet_packet_create(data, strlen(data) + 1, enetMethod);

    enet_peer_send(peer, 0, packet);

    return 0;
}

duk_ret_t networkConnect(duk_context *ctx)
{
    const char *host = duk_require_string(ctx, 0);
    const char *address = duk_require_string(ctx, 1);
    int port = duk_require_number(ctx, 2);

    ENetAddress enetAddress;

    enet_address_set_host(&enetAddress, address);
    enetAddress.port = port;

    ENetHost *enetHost = *map_get(&state.hosts, host);

    ENetPeer *peer = enet_host_connect(enetHost, &enetAddress, 2, 0);

    if (peer == NULL)
    {
        duk_push_error_object(ctx, DUK_ERR_ERROR, "Could not connect to server.");
        duk_throw(ctx);
    }

    char peerId[UUID4_LEN];
    uuid4_generate(peerId);

    map_set(&state.peers, peerId, peer);

    duk_push_string(ctx, peerId);

    return 1;
}

void registerNetworkFunctions(duk_context *ctx)
{
    duk_get_global_string(ctx, "turtle");
    duk_push_object(ctx);
    duk_put_prop_string(ctx, -2, "network");

    duk_pop(ctx);
    duk_pop(ctx);
    duk_pop(ctx);
    duk_pop(ctx);  // UNDERSTAND THE DUKTAPE STACK BETTER TO PREVENT... THIS
    duk_pop(ctx);
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "network");
    duk_push_c_function(ctx, networkNewServer, 2);
    duk_put_prop_string(ctx, -2, "newServer");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "network");
    duk_push_c_function(ctx, networkNewClient, 0);
    duk_put_prop_string(ctx, -2, "newClient");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "network");
    duk_push_c_function(ctx, networkService, 2);
    duk_put_prop_string(ctx, -2, "service");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "network");
    duk_push_c_function(ctx, networkSend, 3);
    duk_put_prop_string(ctx, -2, "send");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "network");
    duk_push_c_function(ctx, networkConnect, 3);
    duk_put_prop_string(ctx, -2, "connect");
    duk_pop(ctx);
}

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

duk_ret_t physicsNewCircleCollider(duk_context *ctx)
{
    int x = duk_require_number(ctx, 0);
    int y = duk_require_number(ctx, 1);
    int radius = duk_require_number(ctx, 2);

    cpFloat mass = 1;
    cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);

    cpBody *body = cpSpaceAddBody(state.space, cpBodyNew(mass, moment));
    cpBodySetPosition(body, cpv(x, y));

    cpShape *shape = cpSpaceAddShape(state.space, cpCircleShapeNew(body, radius, cpvzero));

    Collider collider;
    collider.body = body;
    collider.shape = shape;
    collider.class = "none";

    char colliderId[UUID4_LEN];
    uuid4_generate(colliderId);

    map_set(&state.colliders, colliderId, collider);

    duk_push_string(ctx, colliderId);

    return 1;
}

duk_ret_t physicsNewRectangleCollider(duk_context *ctx)
{
    int x = duk_require_number(ctx, 0);
    int y = duk_require_number(ctx, 1);
    int width = duk_require_number(ctx, 2);
    int height = duk_require_number(ctx, 3);

    cpFloat mass = 1;
    cpFloat moment = cpMomentForBox(mass, width, height);

    cpBody *body = cpSpaceAddBody(state.space, cpBodyNew(mass, moment));
    cpBodySetPosition(body, cpv(x, y));

    cpShape *shape = cpSpaceAddShape(state.space, cpBoxShapeNew(body, width, height, 0));

    Collider collider;
    collider.body = body;
    collider.shape = shape;

    char colliderId[UUID4_LEN];
    uuid4_generate(colliderId);

    map_set(&state.colliders, colliderId, collider);

    duk_push_string(ctx, colliderId);

    return 1;
}

duk_ret_t physicsGetX(duk_context *ctx)
{
    const char *colliderId = duk_require_string(ctx, 0);

    Collider collider = *map_get(&state.colliders, colliderId);

    cpVect pos = cpBodyGetPosition(collider.body);

    duk_push_number(ctx, pos.x);

    return 1;
}

duk_ret_t physicsGetY(duk_context *ctx)
{
    const char *colliderId = duk_require_string(ctx, 0);

    Collider collider = *map_get(&state.colliders, colliderId);

    cpVect pos = cpBodyGetPosition(collider.body);

    duk_push_number(ctx, pos.y);

    return 1;
}

duk_ret_t physicsGetType(duk_context *ctx)
{
    const char *colliderId = duk_require_string(ctx, 0);

    Collider collider = *map_get(&state.colliders, colliderId);

    cpBodyType type = cpBodyGetType(collider.body);

    switch (type)
    {
    case CP_BODY_TYPE_STATIC:
        duk_push_string(ctx, "static");
        break;
    case CP_BODY_TYPE_DYNAMIC:
        duk_push_string(ctx, "dynamic");
        break;
    case CP_BODY_TYPE_KINEMATIC:
        duk_push_string(ctx, "kinematic");
        break;
    }

    return 1;
}

duk_ret_t physicsGetMass(duk_context *ctx)
{
    const char *colliderId = duk_require_string(ctx, 0);

    Collider collider = *map_get(&state.colliders, colliderId);

    cpFloat mass = cpBodyGetMass(collider.body);

    duk_push_number(ctx, mass);

    return 1;
}

duk_ret_t physicsGetFriction(duk_context *ctx)
{
    const char *colliderId = duk_require_string(ctx, 0);

    Collider collider = *map_get(&state.colliders, colliderId);

    cpFloat friction = cpShapeGetFriction(collider.shape);

    duk_push_number(ctx, friction);

    return 1;
}

duk_ret_t physicsSetType(duk_context *ctx)
{
    const char *colliderId = duk_require_string(ctx, 0);

    const char *type = duk_require_string(ctx, 1);

    Collider collider = *map_get(&state.colliders, colliderId);

    if (strcmp("static", type) == 0)
    {
        cpBodySetType(collider.body, CP_BODY_TYPE_STATIC);
    }
    else if (strcmp("dynamic", type) == 0)
    {
        cpBodySetType(collider.body, CP_BODY_TYPE_DYNAMIC);
    }
    else if (strcmp("kinematic", type) == 0)
    {
        cpBodySetType(collider.body, CP_BODY_TYPE_KINEMATIC);
    }

    return 0;
}

duk_ret_t physicsSetX(duk_context *ctx)
{
    const char *colliderId = duk_require_string(ctx, 0);

    int x = duk_require_number(ctx, 1);

    Collider collider = *map_get(&state.colliders, colliderId);

    cpVect pos = cpBodyGetPosition(collider.body);

    cpBodySetPosition(collider.body, cpv(x, pos.y));

    return 0;
}

duk_ret_t physicsSetY(duk_context *ctx)
{
    const char *colliderId = duk_require_string(ctx, 0);

    int y = duk_require_number(ctx, 1);

    Collider collider = *map_get(&state.colliders, colliderId);

    cpVect pos = cpBodyGetPosition(collider.body);

    cpBodySetPosition(collider.body, cpv(pos.x, y));

    return 0;
}

duk_ret_t physicsSetMass(duk_context *ctx)
{
    const char *colliderId = duk_require_string(ctx, 0);

    float mass = duk_require_number(ctx, 1);

    Collider collider = *map_get(&state.colliders, colliderId);

    cpBodySetMass(collider.body, mass);

    return 0;
}

duk_ret_t physicsSetFriction(duk_context *ctx)
{
    const char *colliderId = duk_require_string(ctx, 0);

    float friction = duk_require_number(ctx, 1);

    Collider collider = *map_get(&state.colliders, colliderId);

    cpShapeSetFriction(collider.shape, friction);

    return 0;
}

duk_ret_t physicsIsColliding(duk_context *ctx)
{
    const char *colliderIdA = duk_require_string(ctx, 0);
    const char *colliderIdB = duk_require_string(ctx, 1);

    bool colliding = false;

    int i; Collision val;
    vec_foreach(&state.collisions, val, i) {
        if (strcmp(colliderIdA, val.idA) == 0)
            if (strcmp(colliderIdB, val.idB) == 0)
                colliding = true;
    }

    duk_push_boolean(ctx, colliding);

    return 1;
}

void collision(cpArbiter *arb, cpSpace *space, cpDataPointer data)
{
    cpBody *a;
    cpBody *b;

    cpArbiterGetBodies(arb, &a, &b);

    const char *idA = "";
    const char *idB = "";

    const char *key;
    map_iter_t iter = map_iter(&state.colliders);

    while ((key = map_next(&state.colliders, &iter))) {
        Collider collider = *map_get(&state.colliders, key);

        if (collider.body == a)
            idA = key;
        else if (collider.body == b)
            idB = key;
    }

    Collision collision;
    collision.idA = idA;
    collision.idB = idB;

    vec_push(&state.collisions, collision);
}

void registerPhysicsFunctions(duk_context *ctx)
{
    cpCollisionHandler *handler = cpSpaceAddCollisionHandler(state.space, 0, 0);
    handler->postSolveFunc = (cpCollisionPostSolveFunc)collision;

    duk_get_global_string(ctx, "turtle");
    duk_push_object(ctx);
    duk_put_prop_string(ctx, -2, "physics");

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsNewCircleCollider, 3);
    duk_put_prop_string(ctx, -2, "newCircleCollider");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsNewRectangleCollider, 4);
    duk_put_prop_string(ctx, -2, "newRectangleCollider");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsGetX, 1);
    duk_put_prop_string(ctx, -2, "getX");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsGetY, 1);
    duk_put_prop_string(ctx, -2, "getY");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsGetType, 1);
    duk_put_prop_string(ctx, -2, "getType");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsSetType, 2);
    duk_put_prop_string(ctx, -2, "setType");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsSetX, 2);
    duk_put_prop_string(ctx, -2, "setX");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsSetY, 2);
    duk_put_prop_string(ctx, -2, "setY");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsGetMass, 1);
    duk_put_prop_string(ctx, -2, "getMass");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsSetMass, 2);
    duk_put_prop_string(ctx, -2, "setMass");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsGetFriction, 1);
    duk_put_prop_string(ctx, -2, "getFriction");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsSetFriction, 2);
    duk_put_prop_string(ctx, -2, "setFriction");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsIsColliding, 2);
    duk_put_prop_string(ctx, -2, "isColliding");
    duk_pop(ctx);
}

duk_ret_t systemGetClipboardText(duk_context *ctx)
{
    const char *text = GetClipboardText();

    duk_push_string(ctx, text);

    return 1;
}

duk_ret_t systemGetOS(duk_context *ctx)
{
    char *os;

    #ifdef _WIN32
        os = "Windows";
    #elif _WIN64
        os = "Windows";
    #elif __APPLE__ || __MACH__
        os = "OS X";
    #elif __linux__
        os = "Linux";
    #elif __FreeBSD__
        os = "FreeBSD";
    #else
        os = "Other";
    #endif

    duk_push_string(ctx, os);

    return 1;
}

duk_ret_t systemOpenURL(duk_context *ctx)
{
    const char *url = duk_require_string(ctx, 0);

    OpenURL(url);

    return 0;
}

duk_ret_t systemSetClipboardText(duk_context *ctx)
{
    const char *text = duk_require_string(ctx, 0);

    SetClipboardText(text);

    return 0;
}

void registerSystemFunctions(duk_context *ctx)
{
    duk_get_global_string(ctx, "turtle");
    duk_push_object(ctx);
    duk_put_prop_string(ctx, -2, "system");

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "system");
    duk_push_c_function(ctx, systemGetClipboardText, 0);
    duk_put_prop_string(ctx, -2, "getClipboardText");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "system");
    duk_push_c_function(ctx, systemGetOS, 0);
    duk_put_prop_string(ctx, -2, "getOS");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "system");
    duk_push_c_function(ctx, systemOpenURL, 1);
    duk_put_prop_string(ctx, -2, "openURL");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "system");
    duk_push_c_function(ctx, systemSetClipboardText, 1);
    duk_put_prop_string(ctx, -2, "setClipboardText");
    duk_pop(ctx);
}

duk_ret_t timerGetDelta(duk_context *ctx)
{
    float delta = GetFrameTime();

    duk_push_number(ctx, delta);

    return 1;
}

duk_ret_t timerGetFPS(duk_context *ctx)
{
    int fps = GetFPS();

    duk_push_number(ctx, fps);

    return 1;
}

duk_ret_t timerGetTime(duk_context *ctx)
{
    double time = GetTime();

    duk_push_number(ctx, time);

    return 1;
}

void registerTimerFunctions(duk_context *ctx)
{
    duk_get_global_string(ctx, "turtle");
    duk_push_object(ctx);
    duk_put_prop_string(ctx, -2, "timer");

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "timer");
    duk_push_c_function(ctx, timerGetDelta, 0);
    duk_put_prop_string(ctx, -2, "getDelta");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "timer");
    duk_push_c_function(ctx, timerGetFPS, 0);
    duk_put_prop_string(ctx, -2, "getFPS");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "timer");
    duk_push_c_function(ctx, timerGetTime, 0);
    duk_put_prop_string(ctx, -2, "getTime");
    duk_pop(ctx);
}

duk_ret_t windowClose(duk_context *ctx)
{
    state.close = 1;

    return 0;
}

duk_ret_t windowGetDisplayWidth(duk_context *ctx)
{
    int displayWidth = GetMonitorWidth(GetCurrentMonitor());

    duk_push_number(ctx, displayWidth);

    return 1;
}

duk_ret_t windowGetDisplayHeight(duk_context *ctx)
{
    int displayHeight = GetMonitorHeight(GetCurrentMonitor());

    duk_push_number(ctx, displayHeight);

    return 1;
}

duk_ret_t windowGetWidth(duk_context *ctx)
{
    int width = GetScreenWidth();

    duk_push_number(ctx, width);

    return 1;
}

duk_ret_t windowGetHeight(duk_context *ctx)
{
    int height = GetScreenHeight();

    duk_push_number(ctx, height);

    return 1;
}

duk_ret_t windowGetDisplayName(duk_context *ctx)
{
    const char *displayName = GetMonitorName(GetCurrentMonitor());

    duk_push_string(ctx, displayName);

    return 1;
}

duk_ret_t windowGetFullscreen(duk_context *ctx)
{
    bool fullscreen = IsWindowFullscreen();

    duk_push_boolean(ctx, fullscreen);

    return 1;
}

duk_ret_t windowGetX(duk_context *ctx)
{
    Vector2 position = GetWindowPosition();

    duk_push_number(ctx, position.x);

    return 1;
}

duk_ret_t windowGetY(duk_context *ctx)
{
    Vector2 position = GetWindowPosition();

    duk_push_number(ctx, position.y);

    return 1;
}

duk_ret_t windowGetTitle(duk_context *ctx)
{
    char *title = state.title;

    duk_push_string(ctx, title);

    return 1;
}

duk_ret_t windowGetVSync(duk_context *ctx)
{
    bool vSync = state.vSync;

    duk_push_boolean(ctx, vSync);

    return 1;
}

duk_ret_t windowHasFocus(duk_context *ctx)
{
    bool focus = IsWindowFocused();

    duk_push_boolean(ctx, focus);

    return 1;
}

duk_ret_t windowIsVisible(duk_context *ctx)
{
    bool visible = !IsWindowHidden();

    duk_push_boolean(ctx, visible);

    return 1;
}

duk_ret_t windowIsMaximized(duk_context *ctx)
{
    bool maximized = IsWindowMaximized();

    duk_push_boolean(ctx, maximized);

    return 1;
}

duk_ret_t windowIsMinimized(duk_context *ctx)
{
    bool minimized = IsWindowMinimized();

    duk_push_boolean(ctx, minimized);

    return 1;
}

duk_ret_t windowMaximize(duk_context *ctx)
{
    MaximizeWindow();

    return 0;
}

duk_ret_t windowMinimize(duk_context *ctx)
{
    MinimizeWindow();

    return 0;
}

duk_ret_t windowRestore(duk_context *ctx)
{
    RestoreWindow();

    return 0;
}

duk_ret_t windowSetFullscreen(duk_context *ctx)
{
    bool fullscreen = duk_require_boolean(ctx, 0);

    bool isFullscreen = IsWindowFullscreen();

    if (fullscreen && !isFullscreen)
    {
        ToggleFullscreen();
    }
    else if (!fullscreen && isFullscreen)
    {
        ToggleFullscreen();
    }

    return 0;
}

duk_ret_t windowSetPosition(duk_context *ctx)
{
    int x = duk_require_number(ctx, 0);
    int y = duk_require_number(ctx, 1);

    SetWindowPosition(x, y);

    return 0;
}

duk_ret_t windowSetTitle(duk_context *ctx)
{
    const char *title = duk_require_string(ctx, 0);

    SetWindowTitle(title);

    return 0;
}

duk_ret_t windowSetVSync(duk_context *ctx)
{
    bool vSync = duk_require_boolean(ctx, 0);

    if (vSync)
    {
        SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
        state.vSync = true;
    }
    else
    {
        SetTargetFPS(0);
        state.vSync = false;
    }

    return 0;
}

duk_ret_t windowSetResizable(duk_context *ctx)
{
    bool resizable = duk_require_boolean(ctx, 0);

    if (resizable)
    {
        SetWindowState(FLAG_WINDOW_RESIZABLE);
    }
    else
    {
        ClearWindowState(FLAG_WINDOW_RESIZABLE);
    }

    return 0;
}

duk_ret_t windowIsResized(duk_context *ctx)
{
    bool resized = IsWindowResized();

    duk_push_boolean(ctx, resized);

    return 1;
}

duk_ret_t windowSetMinSize(duk_context *ctx)
{
    int width = duk_require_number(ctx, 0);
    int height = duk_require_number(ctx, 1);

    SetWindowMinSize(width, height);

    return 0;
}

void registerWindowFunctions(duk_context *ctx)
{
    duk_get_global_string(ctx, "turtle");
    duk_push_object(ctx);
    duk_put_prop_string(ctx, -2, "window");

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "window");
    duk_push_c_function(ctx, windowClose, 0);
    duk_put_prop_string(ctx, -2, "close");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "window");
    duk_push_c_function(ctx, windowGetDisplayWidth, 0);
    duk_put_prop_string(ctx, -2, "getDisplayWidth");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "window");
    duk_push_c_function(ctx, windowGetDisplayHeight, 0);
    duk_put_prop_string(ctx, -2, "getDisplayHeight");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "window");
    duk_push_c_function(ctx, windowGetWidth, 0);
    duk_put_prop_string(ctx, -2, "getWidth");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "window");
    duk_push_c_function(ctx, windowGetHeight, 0);
    duk_put_prop_string(ctx, -2, "getHeight");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "window");
    duk_push_c_function(ctx, windowGetDisplayName, 0);
    duk_put_prop_string(ctx, -2, "getDisplayName");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "window");
    duk_push_c_function(ctx, windowGetFullscreen, 0);
    duk_put_prop_string(ctx, -2, "getFullscreen");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "window");
    duk_push_c_function(ctx, windowGetX, 0);
    duk_put_prop_string(ctx, -2, "getX");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "window");
    duk_push_c_function(ctx, windowGetY, 0);
    duk_put_prop_string(ctx, -2, "getY");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "window");
    duk_push_c_function(ctx, windowGetTitle, 0);
    duk_put_prop_string(ctx, -2, "getTitle");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "window");
    duk_push_c_function(ctx, windowGetVSync, 0);
    duk_put_prop_string(ctx, -2, "getVSync");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "window");
    duk_push_c_function(ctx, windowHasFocus, 0);
    duk_put_prop_string(ctx, -2, "hasFocus");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "window");
    duk_push_c_function(ctx, windowIsVisible, 0);
    duk_put_prop_string(ctx, -2, "isVisible");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "window");
    duk_push_c_function(ctx, windowIsMaximized, 0);
    duk_put_prop_string(ctx, -2, "isMaximized");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "window");
    duk_push_c_function(ctx, windowIsMinimized, 0);
    duk_put_prop_string(ctx, -2, "isMinimized");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "window");
    duk_push_c_function(ctx, windowMaximize, 0);
    duk_put_prop_string(ctx, -2, "maximize");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "window");
    duk_push_c_function(ctx, windowMinimize, 0);
    duk_put_prop_string(ctx, -2, "minimize");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "window");
    duk_push_c_function(ctx, windowRestore, 0);
    duk_put_prop_string(ctx, -2, "restore");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "window");
    duk_push_c_function(ctx, windowSetFullscreen, 1);
    duk_put_prop_string(ctx, -2, "setFullscreen");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "window");
    duk_push_c_function(ctx, windowSetPosition, 2);
    duk_put_prop_string(ctx, -2, "setPosition");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "window");
    duk_push_c_function(ctx, windowSetTitle, 1);
    duk_put_prop_string(ctx, -2, "setTitle");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "window");
    duk_push_c_function(ctx, windowSetVSync, 1);
    duk_put_prop_string(ctx, -2, "setVSync");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "window");
    duk_push_c_function(ctx, windowSetResizable, 1);
    duk_put_prop_string(ctx, -2, "setResizable");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "window");
    duk_push_c_function(ctx, windowIsResized, 0);
    duk_put_prop_string(ctx, -2, "isResized");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "window");
    duk_push_c_function(ctx, windowSetMinSize, 2);
    duk_put_prop_string(ctx, -2, "setMinSize");
    duk_pop(ctx);
}

duk_ret_t modSearch(duk_context *ctx)
{
    const char *id = duk_get_string(ctx, 0);

    sds filename = sdsempty();

    filename = sdscatprintf(filename, "%s/%s.js", state.baseDir, id);

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

    if (strcmp(argv[1], "version") == 0)
    {
        printf("TURTLE %s\n", VERSION);
        return 0;
    }

    if (strcmp(argv[1], "help") == 0)
    {
        printf("turtle [path to main.js/ts] [version] [help]\n");
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

    duk_push_object(ctx);
    duk_put_global_string(ctx, "exports");

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
    InitWindow(800, 600, state.title);
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
        strcat(command, " -q -C module.type=commonjs -C module.strictMode=false");

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
        sds command = sdsempty();

        command = sdscatprintf(command, "find %s -name \"*.js\" -type f -delete", state.baseDir);

        if (system(command) != 0)
            printf("Error removing compiled JavaScript files.");
    }

    return 0;
}