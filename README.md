# lua-win-pipe-v2
A lightweight C++ library for Windows Named Pipes in synchronous mode for Lua 5.4. 
It provides a direct wrapper for WinAPI functions with safe resource management and modular structure.
## Features
- **Byte & Message Modes**: Support for continuous data streams or atomic message packets.
- **Resource Safety**: Automatic handle cleanup via Lua Garbage Collector (__gc).
- **Error Handling**: Detailed translation of Windows System Errors into Lua strings.
- **Static Linking**: Built with -static-libgcc -static-libstdc++, no extra runtime dependencies required.
## Installation
1. Download the [latest release][binary-lua54] and unzip.
2. Place folder lua-win-pipe-v_2_0-lua54-win64 containing files init.lua and lua_win_pipe_v2.dll to your project directory (or other place within your package.path).
3. Require the module:
```lua
local lwp = require("lua-win-pipe-v_2_0-lua54-win64.init")
```
## API Reference
### `lwp.server_pipe(name, opts)`
Creates a server pipe instance.
- **opts.mode**: "r", "w", or "rw" (default: "rw")
- **opts.message_mode**: boolean (default: false)
- **opts.first_instance**: boolean (default: true)
- **opts.max_instances**: integer (default: 1)
- **opts.output_buffer_size**: integer (default: 4096)
- **opts.input_buffer_size**: integer (default: 4096)
### `lwp.client_pipe(name, mode)`
Connects to an existing named pipe.
### Common Methods
- **read(n)**: Reads up to n bytes. In Message Mode, returns data, is_incomplete.
- **read_exactly(n)**: Reads exactly n bytes, ignoring message boundaries.
- **write(data)**: Performs a single write operation.
- **write_all(data)**: Ensures the entire string is written (loops if necessary).
- **peek()**: Returns the number of bytes available in the pipe buffer.
- **flush()**: Flushes the write buffers.
- **close()**: Closes the handle. Server pipes will call disconnect before closing.
### Additional server methods
- **server:connect()**: Blocks execution until a client connects to the pipe. Returns true on success.
- **server:disconnect()**: Disconnects the current client without destroying the pipe.
## Examples
Complete usage scenarios are available in:
- [sample-server.lua][sample-server]
- [sample-client.lua][sample-client]
## Downloads
Pre-built binaries for Windows x64 (Lua 5.4):
[Download v.2.0 Release][binary-lua54]

[sample-client]: sample-client.lua
[sample-server]: sample-server.lua
[binary-lua54]:https://github.com/dsabdrashitov/lua-win-pipe-v2/releases/download/v.2.0/lua-win-pipe-v_2_0-lua54-win64.zip

---
<!-- ai-assisted: true -->
<!-- ai-usage: cpp, lua, makefile -->
> [!NOTE]
> This repository is tagged as **AI-assisted**. The project was developed with the technical support of the **Gemini 3 Flash Preview**.
