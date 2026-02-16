local lwp = require("build.lua-win-pipe-v_2_0-lua54-win64.init")
local PIPE_NAME = [[\\.\pipe\MyTestPipe]]

print("[Server] Creating pipe...")
local srv, err = lwp.server_pipe(PIPE_NAME, {
    mode = "rw",
    message_mode = true, -- Тестируем пакетный режим
    first_instance = true
})

if not srv then error("Failed to create server: " .. tostring(err)) end

print("[Server] Waiting for client...")
local ok, err = srv:connect()
if not ok then error("Connect error: " .. tostring(err)) end
print("[Server] Client connected!")

-- 1. Тест Peek и Read
print("[Server] Waiting for data...")
while true do
    local avail = srv:peek()
    if avail and avail > 0 then break end
    if not avail then error("Peek failed") end
end

local data, more = srv:read(5) -- Читаем только часть
print(string.format("[Server] Read partial: '%s', more_data: %s", data, tostring(more)))

if more then
    local rest = srv:read(1024)
    print("[Server] Read rest: " .. rest)
end

-- 2. Тест Write
print("[Server] Sending response...")
srv:write("Hello from Server!")
srv:flush()

-- 3. Тест Read Exactly
print("[Server] Waiting for exact chunk (10 bytes)...")
local chunk = srv:read_exactly(10)
print("[Server] Received exact chunk: " .. chunk)

print("[Server] Disconnecting client...")
srv:disconnect()

print("[Server] Closing...")
srv:close()
print("[Server] Done.")
