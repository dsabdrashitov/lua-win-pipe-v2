#include "errors.h"

namespace lwp::errors {

int push_windows_error(lua_State *L, DWORD last_error) {
    lua_pushnil(L);
    if (last_error == 0) {
        lua_pushstring(L, "[0] No error");
    } else {
        char *message = NULL;
        FormatMessageA(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, last_error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPSTR)&message, 0, NULL);
        if (message) {
            lua_pushfstring(L, "[%d] %s", last_error, message);
            LocalFree(message);
        } else {
            lua_pushfstring(L, "[%d] Error without message", last_error);
        }
    }
    lua_pushinteger(L, (lua_Integer)last_error);
    return 3;
}

}
