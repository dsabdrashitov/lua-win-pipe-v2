#include "library.h"

#include "client_pipe.h"
#include "server_pipe.h"

// DLL entry point
BOOL APIENTRY DllMain(HANDLE hModule, DWORD  fdwReason, LPVOID lpReserved) {
    return TRUE;
}

static const struct luaL_Reg library_functions[] = {
    {"client_pipe", lwp::client_pipe::pipe_open},
    {"server_pipe", lwp::server_pipe::pipe_create},
    {NULL, NULL}
};

extern "C" LUALIB_API int luaopen_lua_win_pipe_v2(lua_State* L) {
    lwp::client_pipe::registerMeta(L);
    lwp::server_pipe::registerMeta(L);
    
    luaL_newlib(L, library_functions);
    return 1;
}
