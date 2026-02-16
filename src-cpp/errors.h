#ifndef LWP_ERRORS_H
#define LWP_ERRORS_H

#include "common.h"

namespace lwp::errors {
    int push_windows_error(lua_State *L, DWORD last_error);
}

#endif
