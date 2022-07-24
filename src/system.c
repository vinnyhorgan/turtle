#include "system.h"

#include "raylib.h"

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