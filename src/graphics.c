#include "graphics.h"

#include "state.h"

#include "raylib.h"

#include "map.h"
#include "uuid4.h"

#include <string.h>

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

    Texture2D image = LoadTexture(filename);

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