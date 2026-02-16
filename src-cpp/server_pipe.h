#ifndef LWP_SERVER_PIPE_H
#define LWP_SERVER_PIPE_H

#include "common.h"

namespace lwp::server_pipe {
    void registerMeta(lua_State* L);
    int pipe_create(lua_State *L);
    int pipe_connect(lua_State *L);
    int pipe_disconnect(lua_State *L);
    int pipe_close(lua_State *L);
}

#endif
