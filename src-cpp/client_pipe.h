#ifndef LWP_CLIENT_PIPE_H
#define LWP_CLIENT_PIPE_H

#include "common.h"

namespace lwp::client_pipe {

    void registerMeta(lua_State* L);
    int pipe_open(lua_State *L);
    int pipe_close(lua_State *L);
    
}

#endif
