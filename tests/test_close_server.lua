local lwp = require("build.lib.lua_win_pipe_v_2_2_lua54_win64")

local PIPE_NAME = [[\\.\pipe\test_pipe]]
local ITERATIONS = 2

print("[SERVER] Starting server...")

-- Create server pipe
local server = lwp.server_pipe(PIPE_NAME, {
    mode = "rw",
    message_mode = false,  -- using byte mode for simplicity
    first_instance = true,
    max_instances = 1
})

if not server then
    print("[SERVER] Failed to create pipe")
    return
end

print("[SERVER] Pipe created successfully")

for i = 1, ITERATIONS do
    print(string.format("[SERVER] Iteration %d: Waiting for client...", i))
    
    local ok, err = server:connect()
    if not ok then
        print(string.format("[SERVER] Connect failed: %s", err))
        break
    end
    
    print("[SERVER] Client connected")
    
    while true do
        local data, err = server:read(1024)
        
        if not data then
            -- Check if this is an error or just pipe closure
            if err and (err:find("BROKEN_PIPE") or err:find("109")) then
                print("[SERVER] Client disconnected (broken pipe)")
            else
                print(string.format("[SERVER] Read error: %s", err or "unknown error"))
            end
            break
        end
        
        if #data > 0 then
            print(string.format("[SERVER] Received: %s", data))
        end
        
        -- If we got empty data, client might have closed the connection
        if #data == 0 then
            print("[SERVER] Empty read, client might be closing")
            break
        end
    end
    
    -- Disconnect client, prepare for next connection
    print("[SERVER] Disconnecting client...")
    local ok, err = server:disconnect()
    if not ok then
        print(string.format("[SERVER] Disconnect failed: %s", err))
    else
        print("[SERVER] Client disconnected successfully")
    end
end

-- Close server pipe
print("[SERVER] Closing server pipe...")
local ok, err = server:close()
if not ok then
    print(string.format("[SERVER] Close failed: %s", err))
else
    print("[SERVER] Server closed successfully")
end

print("[SERVER] Done")