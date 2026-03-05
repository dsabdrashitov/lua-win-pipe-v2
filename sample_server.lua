local lwp = require("build.lib.lua_win_pipe_v_2_2_lua54_win64")
local PIPE_NAME = [[\\.\pipe\MyTestPipe]]

print("[Server] Creating pipe...")
local srv, err = lwp.server_pipe(PIPE_NAME, {
    mode = "rw",
    message_mode = true,  -- test message mode
    first_instance = true
})

if not srv then
    error(string.format("[Server] Failed to create server: %s", err or "unknown error"))
end
print("[Server] Pipe created successfully")

print("[Server] Waiting for client...")
local ok, err = srv:connect()
if not ok then
    error(string.format("[Server] Connect error: %s", err))
end
print("[Server] Client connected!")

-- 1. Test Peek and partial Read
print("[Server] Waiting for data...")
while true do
    local avail, err = srv:peek()
    if not avail then
        error(string.format("[Server] Peek failed: %s", err))
    end
    if avail > 0 then break end
    -- In a real application you might add a small sleep here,
    -- but for this sample it's acceptable to spin.
end

local data, more = srv:read(5)  -- read only first 5 bytes
if not data then
    error(string.format("[Server] Read failed: %s", err))
end
print(string.format("[Server] Read partial: '%s', more_data: %s", data, tostring(more)))

if more then
    local rest, incomplete = srv:read(1024)
    if not rest then
        error(string.format("[Server] Read rest failed: %s", err))
    end
    print("[Server] Read rest: " .. rest)
end

-- 2. Test Write and Flush
print("[Server] Sending response...")
local bytes, err = srv:write("Hello from Server!")
if not bytes then
    error(string.format("[Server] Write failed: %s", err))
else
    print("[Server] Wrote " .. bytes .. " bytes")
end

local ok, err = srv:flush()
if not ok then
    error(string.format("[Server] Flush failed: %s", err))
end

-- 3. Test Read Exactly
print("[Server] Waiting for exact chunk (10 bytes)...")
local chunk, err = srv:read_exactly(10)
if not chunk then
    error(string.format("[Server] read_exactly failed: %s", err))
end
print("[Server] Received exact chunk: " .. chunk)

print("[Server] Disconnecting client...")
local ok, err = srv:disconnect()
if not ok then
    print(string.format("[Server] Disconnect warning: %s", err))
end

print("[Server] Closing...")
local ok, err = srv:close()
if not ok then
    print(string.format("[Server] Close warning: %s", err))
end
print("[Server] Done.")
