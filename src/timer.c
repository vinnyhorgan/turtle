#include "timer.h"

#include "raylib.h"

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