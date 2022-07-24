#include "window.h"

#include "state.h"

#include "raylib.h"

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