local lwp = require("build.lib.lua_win_pipe_v_2_2_lua54_win64")

local PIPE_NAME = [[\\.\pipe\test_pipe]]
local ITERATIONS = 2

local function sleep(seconds)
    local t0 = os.clock()
    while os.clock() - t0 < seconds do end
end

print("[CLIENT] Starting client...")

for i = 1, ITERATIONS do
    print(string.format("[CLIENT] Iteration %d: Connecting to server...", i))
    
    local client, err = lwp.client_pipe(PIPE_NAME, "rw")
    if not client then
        print(string.format("[CLIENT] Failed to connect to server: %s", err or "unknown error"))
        return
    end
    
    print("[CLIENT] Connected successfully")
    
    local message = string.format("Hello from client iteration %d", i)
    print(string.format("[CLIENT] Sending: %s", message))
    
    local success, err = client:write_all(message)
    if not success then
        print(string.format("[CLIENT] Write failed: %s", err))
    else
        print("[CLIENT] Message sent successfully")
    end
    
    sleep(0.1)
    
    print("[CLIENT] Closing connection...")
    local ok, err = client:close()
    if not ok then
        print(string.format("[CLIENT] Close failed: %s", err))
    else
        print("[CLIENT] Connection closed")
    end
    
    if i < ITERATIONS then
        print("[CLIENT] Waiting before next connection...")
        sleep(0.2)  -- 200 ms
    end
end

print("[CLIENT] Done")
