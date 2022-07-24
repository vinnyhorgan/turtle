#include "mouse.h"

#include "state.h"

#include "raylib.h"

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