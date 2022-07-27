#include "network.h"

#include "enet/enet.h"

#include "state.h"

#include "uuid4.h"
#include "map.h"

#include "sds.h"

duk_ret_t networkNewServer(duk_context *ctx)
{
    const char *address = duk_require_string(ctx, 0);
    int port = duk_require_number(ctx, 1);

    ENetAddress enetAddress;
    enet_address_set_host(&enetAddress, address);
    enetAddress.port = port;

    ENetHost *server = enet_host_create(&enetAddress, 32, 2, 0, 0);

    if (server == NULL)
    {
        duk_push_error_object(ctx, DUK_ERR_ERROR, "Could not create server.");
        duk_throw(ctx);
    }

    char hostId[UUID4_LEN];
    uuid4_generate(hostId);

    map_set(&state.hosts, hostId, server);

    duk_push_string(ctx, hostId);

    return 1;
}

duk_ret_t networkNewClient(duk_context *ctx)
{
    ENetHost *client = enet_host_create(NULL, 1, 2, 0, 0);

    if (client == NULL)
    {
        duk_push_error_object(ctx, DUK_ERR_ERROR, "Could not create client.");
        duk_throw(ctx);
    }

    char hostId[UUID4_LEN];
    uuid4_generate(hostId);

    map_set(&state.hosts, hostId, client);

    duk_push_string(ctx, hostId);

    return 1;
}

duk_ret_t networkService(duk_context *ctx)
{
    const char *host = duk_require_string(ctx, 0);
    int timeout = duk_require_number(ctx, 1);

    ENetEvent event;

    ENetHost *enetHost = *map_get(&state.hosts, host);

    enet_host_service(enetHost, &event, timeout);

    sds type = sdsempty();

    switch (event.type)
    {
    case ENET_EVENT_TYPE_CONNECT:
        sdscat(type, "connect");
        break;
    case ENET_EVENT_TYPE_DISCONNECT:
        sdscat(type, "disconnect");
        break;
    case ENET_EVENT_TYPE_RECEIVE:
        sdscat(type, "receive");
        break;
    }

    if (event.type != 0)
    {
        char peerId[UUID4_LEN];
        uuid4_generate(peerId);

        map_set(&state.peers, peerId, event.peer);

        duk_idx_t obj = duk_push_object(ctx);
        duk_push_string(ctx, type);
        duk_put_prop_string(ctx, obj, "type");
        duk_push_string(ctx, peerId);
        duk_put_prop_string(ctx, obj, "peer");

        if (strcmp(type, "receive") == 0)
        {
            duk_push_string(ctx, event.packet->data);
            duk_put_prop_string(ctx, obj, "data");
        }
    }
    else
    {
        duk_idx_t obj = duk_push_object(ctx);
        duk_push_string(ctx, "none");
        duk_put_prop_string(ctx, obj, "type");
    }

    return 1;
}

duk_ret_t networkSend(duk_context *ctx)
{
    const char *peerId = duk_require_string(ctx, 0);
    const char *data = duk_require_string(ctx, 1);
    const char *method = duk_get_string(ctx, 2);

    if (method == NULL)
    {
        method = "reliable";
    }

    ENetPacketFlag enetMethod;

    if (strcmp(data, "reliable") == 0)
    {
        enetMethod = ENET_PACKET_FLAG_RELIABLE;
    }
    else if (strcmp(data, "unreliable") == 0)
    {
        enetMethod = ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT;
    }

    ENetPeer *peer = *map_get(&state.peers, peerId);

    ENetPacket *packet = enet_packet_create(data, strlen(data) + 1, enetMethod);

    enet_peer_send(peer, 0, packet);

    return 0;
}

duk_ret_t networkConnect(duk_context *ctx)
{
    const char *host = duk_require_string(ctx, 0);
    const char *address = duk_require_string(ctx, 1);
    int port = duk_require_number(ctx, 2);

    ENetAddress enetAddress;

    enet_address_set_host(&enetAddress, address);
    enetAddress.port = port;

    ENetHost *enetHost = *map_get(&state.hosts, host);

    ENetPeer *peer = enet_host_connect(enetHost, &enetAddress, 2, 0);

    if (peer == NULL)
    {
        duk_push_error_object(ctx, DUK_ERR_ERROR, "Could not connect to server.");
        duk_throw(ctx);
    }

    char peerId[UUID4_LEN];
    uuid4_generate(peerId);

    map_set(&state.peers, peerId, peer);

    duk_push_string(ctx, peerId);

    return 1;
}

void registerNetworkFunctions(duk_context *ctx)
{
    duk_get_global_string(ctx, "turtle");
    duk_push_object(ctx);
    duk_put_prop_string(ctx, -2, "network");

    duk_pop(ctx);
    duk_pop(ctx);
    duk_pop(ctx);
    duk_pop(ctx);  // UNDERSTAND THE DUKTAPE STACK BETTER TO PREVENT... THIS
    duk_pop(ctx);
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "network");
    duk_push_c_function(ctx, networkNewServer, 2);
    duk_put_prop_string(ctx, -2, "newServer");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "network");
    duk_push_c_function(ctx, networkNewClient, 0);
    duk_put_prop_string(ctx, -2, "newClient");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "network");
    duk_push_c_function(ctx, networkService, 2);
    duk_put_prop_string(ctx, -2, "service");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "network");
    duk_push_c_function(ctx, networkSend, 3);
    duk_put_prop_string(ctx, -2, "send");
    duk_pop(ctx);

    duk_get_global_string(ctx, "turtle");
    duk_get_prop_string(ctx, -1, "network");
    duk_push_c_function(ctx, networkConnect, 3);
    duk_put_prop_string(ctx, -2, "connect");
    duk_pop(ctx);
}