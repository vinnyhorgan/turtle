#include "camera.h"

#include "raylib.h"

#include "map.h"
#include "uuid4.h"

#include "state.h"

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