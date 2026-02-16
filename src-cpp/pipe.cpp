#include "pipe.h"
#include <vector>

namespace lwp::pipe {

    PipeWrapper* check_pipe(lua_State* L, int arg) {
        void* ud;
        
        ud = luaL_testudata(L, arg, lwp::server_pipe::METATABLE_NAME);
        if (ud) return (PipeWrapper*)ud;

        ud = luaL_testudata(L, arg, lwp::client_pipe::METATABLE_NAME);
        if (ud) return (PipeWrapper*)ud;

        luaL_typeerror(L, arg, "pipe (server or client)");
        return NULL;
    }

    int pipe_read(lua_State* L) {
        PipeWrapper* pw = check_pipe(L, 1);
        if (pw->hPipe == INVALID_HANDLE_VALUE) {
            lua_pushnil(L);
            lua_pushstring(L, "attempt to read from a closed pipe");
            return 2;
        }

        lua_Integer count = luaL_optinteger(L, 2, DEFAULT_PIPE_BUF_SIZE);
        if (count < 0) {
            return luaL_argerror(L, 2, "count must be non-negative");
        }

        // Временный буфер (vector безопасен и удобен)
        std::vector<char> buffer((size_t)count);
        DWORD bytesRead = 0;

        BOOL success = ReadFile(
            pw->hPipe,
            buffer.data(),
            (DWORD)count,
            &bytesRead,
            NULL
        );

        if (!success) {
            DWORD err = GetLastError();
            if (err == ERROR_MORE_DATA) {
                lua_pushlstring(L, buffer.data(), bytesRead);
                lua_pushboolean(L, TRUE); // Индикатор: сообщение прочитано не полностью
                return 2;
            }
            if (err == ERROR_BROKEN_PIPE) {
                // Принудительно закрываем дескриптор, так как он больше не валиден
                close_handle(pw); 
            }
            // Возвращаем nil, "сообщение", код
            return lwp::errors::push_windows_error(L, err);
        }

        // Возвращаем прочитанные данные как строку
        lua_pushlstring(L, buffer.data(), bytesRead);
        return 1;
    }

    int pipe_read_exactly(lua_State* L) {
        PipeWrapper* pw = check_pipe(L, 1);
        if (pw->hPipe == INVALID_HANDLE_VALUE) {
            lua_pushnil(L);
            lua_pushstring(L, "attempt to read from a closed pipe");
            return 2;
        }

        lua_Integer count = luaL_checkinteger(L, 2);
        if (count < 0) {
            return luaL_argerror(L, 2, "count must be non-negative");
        }
        if (count == 0) {
            if (!ReadFile(pw->hPipe, NULL, 0, NULL, NULL)) {
                DWORD err = GetLastError();
                if (err == ERROR_BROKEN_PIPE) close_handle(pw);
                return lwp::errors::push_windows_error(L, err);
            }
            lua_pushstring(L, ""); // Возвращаем пустую строку, как в стандартном io.read(0)
            return 1;
        }

        std::vector<char> buffer((size_t)count);
        size_t totalRead = 0;

        while (totalRead < (size_t)count) {
            DWORD read = 0;
            if (!ReadFile(pw->hPipe, buffer.data() + totalRead, (DWORD)(count - totalRead), &read, NULL)) {
                DWORD err = GetLastError();
                if (err == ERROR_MORE_DATA) {
                    // Игнорируем: граница сообщения достигнута, но нам нужно больше байт.
                    // Windows заполнила текущий кусок буфера, продолжаем цикл.
                } else {
                    if (err == ERROR_BROKEN_PIPE) close_handle(pw);
                    return lwp::errors::push_windows_error(L, err);
                }
            }
            if (read == 0) break;
            totalRead += read;
        }
        

        if (totalRead < (size_t)count) {
            lua_pushnil(L);
            lua_pushfstring(L, "incomplete read (%d of %d bytes)", (int)totalRead, (int)count);
            return 2;
        }

        lua_pushlstring(L, buffer.data(), totalRead);
        return 1;
    }

    int pipe_peek(lua_State* L) {
        PipeWrapper* pw = check_pipe(L, 1);
        
        if (pw->hPipe == INVALID_HANDLE_VALUE) {
            lua_pushnil(L);
            lua_pushstring(L, "pipe is closed");
            return 2;
        }

        DWORD bytesAvail = 0;
        BOOL success = PeekNamedPipe(
            pw->hPipe, 
            NULL, 
            0, 
            NULL, 
            &bytesAvail, 
            NULL
        );

        if (!success) {
            return lwp::errors::push_windows_error(L, GetLastError());
        }

        lua_pushinteger(L, (lua_Integer)bytesAvail);
        return 1;
    }

    int pipe_write(lua_State* L) {
        PipeWrapper* pw = check_pipe(L, 1);
        if (pw->hPipe == INVALID_HANDLE_VALUE) {
            lua_pushnil(L);
            lua_pushstring(L, "attempt to write to a closed pipe");
            return 2;
        }

        size_t len;
        const char* data = luaL_checklstring(L, 2, &len);

        DWORD bytesWritten = 0;
        BOOL success = WriteFile(pw->hPipe, data, (DWORD)len, &bytesWritten, NULL);

        if (!success) {
            return lwp::errors::push_windows_error(L, GetLastError());
        }

        // Проверка на "нулевую" запись при ненулевых входных данных
        if (len > 0 && bytesWritten == 0) {
            lua_pushnil(L);
            lua_pushfstring(L, "write failed (%d of %d bytes)", (int)bytesWritten, (int)len);
            return 2;
        }

        lua_pushinteger(L, (lua_Integer)bytesWritten);
        return 1;
    }

    int pipe_write_all(lua_State* L) {
        PipeWrapper* pw = check_pipe(L, 1);
        if (pw->hPipe == INVALID_HANDLE_VALUE) {
            lua_pushnil(L);
            lua_pushstring(L, "attempt to write to a closed pipe");
            return 2;
        }

        size_t len;
        const char* data = luaL_checklstring(L, 2, &len);

        size_t totalWritten = 0;
        while (totalWritten < len || len == 0) {
            DWORD written = 0;
            if (!WriteFile(pw->hPipe, data + totalWritten, (DWORD)(len - totalWritten), &written, NULL)) {
                return lwp::errors::push_windows_error(L, GetLastError());
            }
            
            if (written == 0) {
                break;
            }
            totalWritten += written;
        }

        if (totalWritten != len) {
            lua_pushnil(L);
            lua_pushfstring(L, "write failed (%d of %d bytes)", (int)totalWritten, (int)len);
            return 2;
        }

        lua_pushboolean(L, TRUE);
        return 1;
    }

    int pipe_flush(lua_State* L) {
        PipeWrapper *pw = check_pipe(L, 1);
        
        if (!FlushFileBuffers(pw->hPipe)) {
            return lwp::errors::push_windows_error(L, GetLastError());
        }

        lua_pushboolean(L, TRUE);
        return 1;
    }

    BOOL close_handle(PipeWrapper *pw) {
        HANDLE handle = pw->hPipe;
        if (handle != INVALID_HANDLE_VALUE) {
            pw->hPipe = INVALID_HANDLE_VALUE; // Reset first for thread-safety/GC
            return CloseHandle(handle);       // Use the saved handle!
        }
        return TRUE;
    }

    int pipe_gc(lua_State *L) {
        PipeWrapper* pw = check_pipe(L, 1);
        close_handle(pw);
        return 0;
    }

}