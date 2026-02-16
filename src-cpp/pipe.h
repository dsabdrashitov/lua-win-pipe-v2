#ifndef LWP_PIPE_H
#define LWP_PIPE_H

#include "common.h"
#include "errors.h"
#include <windows.h>

namespace lwp::client_pipe {
    extern const char* const METATABLE_NAME;
}

namespace lwp::server_pipe {
    extern const char* const METATABLE_NAME;
}

namespace lwp::pipe {

    inline constexpr size_t DEFAULT_PIPE_BUF_SIZE = 4096;
    
    typedef struct {
        HANDLE hPipe;
    } PipeWrapper;

    PipeWrapper* check_pipe(lua_State* L, int arg);
    BOOL close_handle(PipeWrapper *pw);
    
    int pipe_peek(lua_State* L);
    int pipe_read(lua_State* L);
    int pipe_read_exactly(lua_State* L);
    int pipe_write(lua_State* L);
    int pipe_write_all(lua_State* L);
    int pipe_flush(lua_State* L);

    int pipe_gc(lua_State* L);

}

#endif
