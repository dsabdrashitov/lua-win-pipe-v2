Library Context: lua-win-pipe-v2 (Named Pipes for Lua 5.4)

## Overview
lua-win-pipe-v2 is a synchronous C++/Lua wrapper for Windows Named Pipes. It provides a robust, modular interface for Inter-Process Communication (IPC) using native WinAPI calls (CreateNamedPipe, ConnectNamedPipe, ReadFile, etc.).

## Project Architecture
- **Language**: C++17 and Lua 5.4.
- **Library Path**: Typically required as a package (e.g., `local lwp = require("lua_win_pipe_v_2_2_lua54_win64")`).
- **Mode**: Synchronous (blocking) IO using `PIPE_WAIT`.
- **Handle Management**: Uses a `PipeWrapper` structure with automatic cleanup via Lua's `__gc` metamethod.

## API Reference: Constructors

### `lwp.server_pipe(name, opts)`
Creates a server-side instance of a named pipe.
- **name**: String (e.g., `[[\\.\pipe\MyPipeName]]`).
- **opts**: Table of options:
  - `mode`: `"r"`, `"w"`, or `"rw"` (default: `"rw"`).
  - `message_mode`: boolean (true = Message Mode / atomic packets, false = Byte Mode / continuous stream).
  - `first_instance`: boolean (default: true). If true, fails if a pipe with the same name already exists.
  - `max_instances`: int (default: 1).
  - `output_buffer_size`: int (default: 4096).
  - `input_buffer_size`: int (default: 4096).

### `lwp.client_pipe(name, mode)`
Connects to an existing named pipe as a client.
- **name**: String name of the pipe.
- **mode**: `"r"`, `"w"`, or `"rw"` (default: `"rw"`).

## API Reference: Common Methods (Client & Server)

- **`pipe:read(n)`**: Reads up to `n` bytes.
  - In Message Mode: Returns `data` (string), `is_incomplete` (boolean). If `is_incomplete` is true, the message buffer was too small and more data remains in the current packet.
  - In Byte Mode: Returns `data` (string).
- **`pipe:read_exactly(n)`**: Reads exactly `n` bytes. It loops until the buffer is full, ignoring message boundaries. Ideal for fixed-size headers.
- **`pipe:write(data)`**: Single attempt to write a string. Returns the number of bytes written.
- **`pipe:write_all(data)`**: Ensures the entire string is written by looping until completion. Returns `true` on success.
- **`pipe:peek()`**: Returns the number of bytes available in the pipe buffer without removing them.
- **`pipe:flush()`**: Flushes the write buffers to the pipe.
- **`pipe:close()`**: Explicitly closes the handle. For servers, it calls `disconnect()` before closing.

## API Reference: Server-Specific Methods

- **`server:connect()`**: Blocks execution until a client connects. Returns `true` on success, or `nil, err_msg, win_code` on failure.
- **`server:disconnect()`**: Disconnects the current client but keeps the pipe handle alive for the next `connect()` call.

## Error Handling
Functions return `nil, error_message, windows_error_code` on failure.

### Common Windows Error Codes:
- **231 (ERROR_PIPE_BUSY)**: All pipe instances are busy.
- **109 (ERROR_BROKEN_PIPE)**: The pipe has been closed by the other end.
- **535 (ERROR_PIPE_CONNECTED)**: A client connected before `connect()` was called (handled internally, but useful to know).

## Technical Implementation Details
- **Safety**: Handles are set to `INVALID_HANDLE_VALUE` immediately after closing to prevent double-free errors.
- **Polymorphism**: The `check_pipe` helper in C++ validates objects against both client and server metatables, allowing shared IO logic.
- **Zero-byte Writes**: The library supports zero-byte writes, which are treated as empty messages in Message Mode.
- **Proxy Loading**: The library uses a Lua proxy file to allow flexible directory structures via `...` path resolution.
