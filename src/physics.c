#include "physics.h"

#include "chipmunk/chipmunk.h"

#include "state.h"

#include "raylib.h"

#include "map.h"
#include "uuid4.h"

#include <string.h>

duk_ret_t physicsNewCircleCollider(duk_context *ctx)
{
    int x = duk_require_number(ctx, 0);
    int y = duk_require_number(ctx, 1);
    int radius = duk_require_number(ctx, 2);

    cpFloat mass = 1;
    cpFloat moment = cpMomentForCircle(mass, 0, radius, cpvzero);

    cpBody *body = cpSpaceAddBody(state.space, cpBodyNew(mass, moment));
    cpBodySetPosition(body, cpv(x, y));

    cpShape *shape = cpSpaceAddShape(state.space, cpCircleShapeNew(body, radius, cpvzero));

    Collider collider;
    collider.body = body;
    collider.shape = shape;
    collider.class = "none";

    char colliderId[UUID4_LEN];
    uuid4_generate(colliderId);

    map_set(&state.colliders, colliderId, collider);

    duk_push_string(ctx, colliderId);

    return 1;
}

duk_ret_t physicsNewRectangleCollider(duk_context *ctx)
{
    int x = duk_require_number(ctx, 0);
    int y = duk_require_number(ctx, 1);
    int width = duk_require_number(ctx, 2);
    int height = duk_require_number(ctx, 3);

    cpFloat mass = 1;
    cpFloat moment = cpMomentForBox(mass, width, height);

    cpBody *body = cpSpaceAddBody(state.space, cpBodyNew(mass, moment));
    cpBodySetPosition(body, cpv(x, y));

    cpShape *shape = cpSpaceAddShape(state.space, cpBoxShapeNew(body, width, height, 0));

    Collider collider;
    collider.body = body;
    collider.shape = shape;

    char colliderId[UUID4_LEN];
    uuid4_generate(colliderId);

    map_set(&state.colliders, colliderId, collider);

    duk_push_string(ctx, colliderId);

    return 1;
}

duk_ret_t physicsGetX(duk_context *ctx)
{
    const char *colliderId = duk_require_string(ctx, 0);

    Collider collider = *map_get(&state.colliders, colliderId);

    cpVect pos = cpBodyGetPosition(collider.body);

    duk_push_number(ctx, pos.x);

    return 1;
}

duk_ret_t physicsGetY(duk_context *ctx)
{
    const char *colliderId = duk_require_string(ctx, 0);

    Collider collider = *map_get(&state.colliders, colliderId);

    cpVect pos = cpBodyGetPosition(collider.body);

    duk_push_number(ctx, pos.y);

    return 1;
}

duk_ret_t physicsGetType(duk_context *ctx)
{
    const char *colliderId = duk_require_string(ctx, 0);

    Collider collider = *map_get(&state.colliders, colliderId);

    cpBodyType type = cpBodyGetType(collider.body);

    switch (type)
    {
    case CP_BODY_TYPE_STATIC:
        duk_push_string(ctx, "static");
        break;
    case CP_BODY_TYPE_DYNAMIC:
        duk_push_string(ctx, "dynamic");
        break;
    case CP_BODY_TYPE_KINEMATIC:
        duk_push_string(ctx, "kinematic");
        break;
    }

    return 1;
}

duk_ret_t physicsGetMass(duk_context *ctx)
{
    const char *colliderId = duk_require_string(ctx, 0);

    Collider collider = *map_get(&state.colliders, colliderId);

    cpFloat mass = cpBodyGetMass(collider.body);

    duk_push_number(ctx, mass);

    return 1;
}

duk_ret_t physicsGetFriction(duk_context *ctx)
{
    const char *colliderId = duk_require_string(ctx, 0);

    Collider collider = *map_get(&state.colliders, colliderId);

    cpFloat friction = cpShapeGetFriction(collider.shape);

    duk_push_number(ctx, friction);

    return 1;
}

duk_ret_t physicsSetType(duk_context *ctx)
{
    const char *colliderId = duk_require_string(ctx, 0);

    const char *type = duk_require_string(ctx, 1);

    Collider collider = *map_get(&state.colliders, colliderId);

    if (strcmp("static", type) == 0)
    {
        cpBodySetType(collider.body, CP_BODY_TYPE_STATIC);
    }
    else if (strcmp("dynamic", type) == 0)
    {
        cpBodySetType(collider.body, CP_BODY_TYPE_DYNAMIC);
    }
    else if (strcmp("kinematic", type) == 0)
    {
        cpBodySetType(collider.body, CP_BODY_TYPE_KINEMATIC);
    }

    return 0;
}

duk_ret_t physicsSetX(duk_context *ctx)
{
    const char *colliderId = duk_require_string(ctx, 0);

    int x = duk_require_number(ctx, 1);

    Collider collider = *map_get(&state.colliders, colliderId);

    cpVect pos = cpBodyGetPosition(collider.body);

    cpBodySetPosition(collider.body, cpv(x, pos.y));

    return 0;
}

duk_ret_t physicsSetY(duk_context *ctx)
{
    const char *colliderId = duk_require_string(ctx, 0);

    int y = duk_require_number(ctx, 1);

    Collider collider = *map_get(&state.colliders, colliderId);

    cpVect pos = cpBodyGetPosition(collider.body);

    cpBodySetPosition(collider.body, cpv(pos.x, y));

    return 0;
}

duk_ret_t physicsSetMass(duk_context *ctx)
{
    const char *colliderId = duk_require_string(ctx, 0);

    float mass = duk_require_number(ctx, 1);

    Collider collider = *map_get(&state.colliders, colliderId);

    cpBodySetMass(collider.body, mass);

    return 0;
}

duk_ret_t physicsSetFriction(duk_context *ctx)
{
    const char *colliderId = duk_require_string(ctx, 0);

    float friction = duk_require_number(ctx, 1);

    Collider collider = *map_get(&state.colliders, colliderId);

    cpShapeSetFriction(collider.shape, friction);

    return 0;
}

duk_ret_t physicsSetCollisionClass(duk_context *ctx)
{
    const char *colliderId = duk_require_string(ctx, 0);

    const char *class = duk_require_string(ctx, 1);

    Collider collider = *map_get(&state.colliders, colliderId);

    collider.class = class;

    return 0;
}

duk_ret_t physicsIsColliding(duk_context *ctx)
{
    const char *colliderIdA = duk_require_string(ctx, 0);
    const char *colliderIdB = duk_require_string(ctx, 1);

    bool colliding = false;

    int i; Collision val;
    vec_foreach(&state.collisions, val, i) {
        if (strcmp(colliderIdA, val.idA) == 0)
            if (strcmp(colliderIdB, val.idB) == 0)
                colliding = true;
    }

    duk_push_boolean(ctx, colliding);

    return 1;
}

void collision(cpArbiter *arb, cpSpace *space, cpDataPointer data)
{
    cpBody *a;
    cpBody *b;

    cpArbiterGetBodies(arb, &a, &b);

    const char *idA;
    const char *idB;

    const char *key;
    map_iter_t iter = map_iter(&state.colliders);

    while ((key = map_next(&state.colliders, &iter))) {
        Collider collider = *map_get(&state.colliders, key);

        if (collider.body == a)
            idA = key;
        else if (collider.body == b)
            idB = key;
    }

    Collision collision;
    collision.idA = idA;
    collision.idB = idB;

    vec_push(&state.collisions, collision);
}

void registerPhysicsFunctions(duk_context *ctx)
{
    cpCollisionHandler *handler = cpSpaceAddCollisionHandler(state.space, 0, 0);
    handler->postSolveFunc = (cpCollisionPostSolveFunc)collision;

    duk_get_global_string(ctx, "turtle");
    duk_push_object(ctx);
    duk_put_prop_string(ctx, -2, "physics");

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsNewCircleCollider, 3);
    duk_put_prop_string(ctx, -2, "newCircleCollider");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsNewRectangleCollider, 4);
    duk_put_prop_string(ctx, -2, "newRectangleCollider");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsGetX, 1);
    duk_put_prop_string(ctx, -2, "getX");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsGetY, 1);
    duk_put_prop_string(ctx, -2, "getY");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsGetType, 1);
    duk_put_prop_string(ctx, -2, "getType");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsSetType, 2);
    duk_put_prop_string(ctx, -2, "setType");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsSetX, 2);
    duk_put_prop_string(ctx, -2, "setX");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsSetY, 2);
    duk_put_prop_string(ctx, -2, "setY");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsGetMass, 1);
    duk_put_prop_string(ctx, -2, "getMass");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsSetMass, 2);
    duk_put_prop_string(ctx, -2, "setMass");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsGetFriction, 1);
    duk_put_prop_string(ctx, -2, "getFriction");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsSetFriction, 2);
    duk_put_prop_string(ctx, -2, "setFriction");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsSetCollisionClass, 2);
    duk_put_prop_string(ctx, -2, "setCollisionClass");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "physics");
    duk_push_c_function(ctx, physicsIsColliding, 2);
    duk_put_prop_string(ctx, -2, "isColliding");
    duk_pop(ctx);
}