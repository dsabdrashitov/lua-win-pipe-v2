local lwp = require("build.lib.lua_win_pipe_v_2_2_lua54_win64")
local PIPE_NAME = [[\\.\pipe\MyTestPipe]]

print("[Client] Connecting to pipe...")
local cli, err = lwp.client_pipe(PIPE_NAME, "rw")
if not cli then
    error(string.format("[Client] Failed to connect: %s", err or "unknown error"))
end
print("[Client] Connected!")

-- 1. Send a message to the server
print("[Client] Writing message...")
local bytes, err = cli:write("This is a long message to test message mode")
if not bytes then
    error(string.format("[Client] Write failed: %s", err))
else
    print("[Client] Wrote " .. bytes .. " bytes")
end

-- 2. Read the server's response (blocking)
print("[Client] Waiting for server response...")
local resp, incomplete = cli:read(1024)  -- read up to 1024 bytes
if not resp then
    error(string.format("[Client] Read failed: %s", err))
else
    print("[Client] Server said: " .. resp)
    if incomplete then
        print("[Client] Warning: message incomplete, more data available")
    end
end

-- 3. Send an exact number of bytes using write_all
print("[Client] Sending 10 bytes exactly...")
local ok, err = cli:write_all("1234567890EXTRA")  -- server will read only first 10
if not ok then
    error(string.format("[Client] Write_all failed: %s", err))
else
    print("[Client] Write_all succeeded")
end

-- 4. Close the connection
print("[Client] Closing...")
local ok, err = cli:close()
if not ok then
    error(string.format("[Client] Close failed: %s", err))
end
print("[Client] Done.")
