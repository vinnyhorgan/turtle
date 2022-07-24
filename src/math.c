#include "math.h"

#include "raylib.h"

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