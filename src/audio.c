#include "audio.h"

#include "raylib.h"

#include "map.h"
#include "uuid4.h"

#include "state.h"

#include <string.h>

duk_ret_t audioNewSource(duk_context *ctx)
{
    const char *filename = duk_require_string(ctx, 0);

    char path[1000];
    strcpy(path, state.baseDir);
    strcat(path, "/");
    strcat(path, filename);

    Sound sound = LoadSound(path);

    char soundId[UUID4_LEN];
    uuid4_generate(soundId);

    map_set(&state.sounds, soundId, sound);

    duk_push_string(ctx, soundId);

    return 1;
}

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