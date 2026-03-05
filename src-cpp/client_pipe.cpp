#include "client_pipe.h"
#include "pipe.h"
#include "errors.h"

namespace lwp::client_pipe {

    extern const char* const METATABLE_NAME = "lwp.client_pipe";

    int pipe_open(lua_State *L) {
        const char *name = luaL_checkstring(L, 1);
        
        DWORD dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
        if (lua_isstring(L, 2)) {
            const char *mode = lua_tostring(L, 2);
            if (strcmp(mode, "r") == 0) {
                dwDesiredAccess = GENERIC_READ;
            } else if (strcmp(mode, "w") == 0) {
                dwDesiredAccess = GENERIC_WRITE;
            } else if (strcmp(mode, "rw") == 0) {
                dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
            } else {
                return luaL_argerror(L, 2, "invalid access mode (expected 'r', 'w', or 'rw')");
            }
        }

        lwp::pipe::PipeWrapper *pw = (lwp::pipe::PipeWrapper *)lua_newuserdata(L, sizeof(lwp::pipe::PipeWrapper));
        pw->hPipe = INVALID_HANDLE_VALUE;

        luaL_getmetatable(L, METATABLE_NAME);
        lua_setmetatable(L, -2);

        pw->hPipe = CreateFileA(
            name,
            dwDesiredAccess,
            0,              // Shared mode: 0 for pipes (exclusive access)
            NULL,           // Security attributes
            OPEN_EXISTING,  // Pipe must already be created by the server
            0,              // Flags and attributes
            NULL
        );

        if (pw->hPipe == INVALID_HANDLE_VALUE) {
            return lwp::errors::push_windows_error(L, GetLastError());
        }

        return 1; 
    }

    int pipe_close(lua_State *L) {
        lwp::pipe::PipeWrapper *pw = lwp::pipe::check_pipe(L, 1);
        if (!lwp::pipe::close_handle(pw)) {
            return lwp::errors::push_windows_error(L, GetLastError());
        }
        lua_pushboolean(L, TRUE);
        return 1;
    }

    static const luaL_Reg client_pipe_methods[] = {
        {"peek",            lwp::pipe::pipe_peek},
        {"read",            lwp::pipe::pipe_read},
        {"read_exactly",    lwp::pipe::pipe_read_exactly},
        {"write",           lwp::pipe::pipe_write},
        {"write_all",       lwp::pipe::pipe_write_all},
        {"flush",           lwp::pipe::pipe_flush},
        {"close",           pipe_close},
        {NULL, NULL}
    };

    void registerMeta(lua_State* L) {
        luaL_newmetatable(L, METATABLE_NAME); // [mt]

        // Set __gc directly in the metatable
        lua_pushcfunction(L, lwp::pipe::pipe_gc);
        lua_setfield(L, -2, "__gc");          // [mt]

        // Create a separate table for user methods
        lua_newtable(L);                      // [mt, methods]
        luaL_setfuncs(L, client_pipe_methods, 0);

        // Set methods table as __index for the metatable
        // Now: mt.__index = methods
        lua_setfield(L, -2, "__index");       // [mt]

        // Clear the stack
        lua_pop(L, 1);
    }
    
}
