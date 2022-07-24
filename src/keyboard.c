#include "keyboard.h"

#include "state.h"

#include "raylib.h"

#include "map.h"

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