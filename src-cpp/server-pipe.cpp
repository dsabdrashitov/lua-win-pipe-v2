#include "server_pipe.h"
#include "pipe.h"
#include "errors.h"

namespace lwp::server_pipe {

    extern const char* const METATABLE_NAME = "lwp.server_pipe";

    int pipe_create(lua_State *L) {
        const char *name        = luaL_checkstring(L, 1);
        const char *mode        = luaL_checkstring(L, 2);
        bool type_message       = lua_toboolean(L, 3);
        bool first_instance     = lua_toboolean(L, 4);
        DWORD max_instances     = (DWORD)luaL_checkinteger(L, 5);
        DWORD out_buf_size      = (DWORD)luaL_checkinteger(L, 6);
        DWORD in_buf_size       = (DWORD)luaL_checkinteger(L, 7);

        DWORD dwOpenMode = 0;
        DWORD dwPipeMode = PIPE_WAIT; // Always blocking

        if (strcmp(mode, "r") == 0) {
            dwOpenMode |= PIPE_ACCESS_INBOUND;
        } else if (strcmp(mode, "w") == 0) {
            dwOpenMode |= PIPE_ACCESS_OUTBOUND;
        } else if (strcmp(mode, "rw") == 0) {
            dwOpenMode |= PIPE_ACCESS_DUPLEX;
        } else {
            return luaL_argerror(L, 2, "invalid access mode (expected 'r', 'w', or 'rw')");
        }

        if (type_message) {
            dwPipeMode |= (PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE);
        } else {
            dwPipeMode |= (PIPE_TYPE_BYTE | PIPE_READMODE_BYTE);
        }

        if (first_instance) dwOpenMode |= FILE_FLAG_FIRST_PIPE_INSTANCE;

        // --- Create object ---
        lwp::pipe::PipeWrapper *pw = (lwp::pipe::PipeWrapper *)lua_newuserdata(L, sizeof(lwp::pipe::PipeWrapper));
        pw->hPipe = INVALID_HANDLE_VALUE;
        luaL_getmetatable(L, METATABLE_NAME);
        lua_setmetatable(L, -2);

        pw->hPipe = CreateNamedPipeA(
            name, dwOpenMode, dwPipeMode, 
            max_instances, out_buf_size, in_buf_size, 
            0, NULL
        );

        if (pw->hPipe == INVALID_HANDLE_VALUE) {
            return lwp::errors::push_windows_error(L, GetLastError());
        }

        return 1;
    }

    int pipe_connect(lua_State *L) {
        lwp::pipe::PipeWrapper *pw = lwp::pipe::check_pipe(L, 1);
        
        // ConnectNamedPipe returns FALSE if a client connected between calls
        BOOL success = ConnectNamedPipe(pw->hPipe, NULL);
        if (!success) {
            DWORD last_error = GetLastError();
            if (last_error == ERROR_PIPE_CONNECTED) {
                success = TRUE;
            } else {
                return lwp::errors::push_windows_error(L, last_error);
            }
        }

        lua_pushboolean(L, TRUE);
        return 1;
    }

    int pipe_disconnect(lua_State *L) {
        lwp::pipe::PipeWrapper *pw = lwp::pipe::check_pipe(L, 1);
        if (!DisconnectNamedPipe(pw->hPipe)) {
            return lwp::errors::push_windows_error(L, GetLastError());
        }
        lua_pushboolean(L, TRUE);
        return 1;
    }

    int pipe_close(lua_State *L) {
        lwp::pipe::PipeWrapper *pw = lwp::pipe::check_pipe(L, 1);
        
        if (pw->hPipe == INVALID_HANDLE_VALUE) {
            lua_pushboolean(L, TRUE);
            return 1;
        }

        DWORD first_error = 0;

        if (!DisconnectNamedPipe(pw->hPipe)) {
            DWORD err = GetLastError();
            if (err != ERROR_PIPE_NOT_CONNECTED) {
                first_error = err;
            }
        }

        if (!lwp::pipe::close_handle(pw)) {
            if (first_error == 0) {
                first_error = GetLastError();
            }
        }

        if (first_error != 0) {
            return lwp::errors::push_windows_error(L, first_error);
        }

        lua_pushboolean(L, TRUE);
        return 1;
    }

    static const luaL_Reg server_pipe_methods[] = {
        {"connect",         pipe_connect},
        {"disconnect",      pipe_disconnect},
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
        luaL_newmetatable(L, METATABLE_NAME);
        lua_pushcfunction(L, lwp::pipe::pipe_gc);
        lua_setfield(L, -2, "__gc");

        lua_newtable(L);
        luaL_setfuncs(L, server_pipe_methods, 0);
        lua_setfield(L, -2, "__index");
        lua_pop(L, 1);
    }
}
