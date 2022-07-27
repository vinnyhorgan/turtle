#ifndef STATE_H
#define STATE_H

#include "map.h"
#include "vec.h"

#include "chipmunk/chipmunk.h"

#include <stdbool.h>

#include "raylib.h"

#include "enet/enet.h"

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

extern State state;

#endif