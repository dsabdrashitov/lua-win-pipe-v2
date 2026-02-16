#ifndef LWP_COMMON_H
#define LWP_COMMON_H

#include <windows.h>

// Lua marks for building as DLL
#define LUA_LIB
#define LUA_BUILD_AS_DLL
 
// Lua headers
extern "C" {
    #include <lauxlib.h>
    #include <lua.h>
}

#endif
