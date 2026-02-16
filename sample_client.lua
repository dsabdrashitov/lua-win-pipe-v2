local lwp = require("build.lua-win-pipe-v_2_0-lua54-win64.init")
local PIPE_NAME = [[\\.\pipe\MyTestPipe]]

print("[Client] Connecting to pipe...")
local cli, err = lwp.client_pipe(PIPE_NAME, "rw")
if not cli then error("Failed to connect: " .. tostring(err)) end
print("[Client] Connected!")

-- 1. Тест Write (создаем сообщение)
print("[Client] Writing message...")
cli:write("This is a long message to test message mode")

-- 2. Тест Read
print("[Client] Waiting for server response...")
local resp = cli:read()
print("[Client] Server said: " .. tostring(resp))

-- 3. Тест Write All
print("[Client] Sending 10 bytes exactly...")
cli:write_all("1234567890EXTRA") -- Пишем больше, сервер заберет только 10

print("[Client] Closing...")
cli:close()
print("[Client] Done.")
