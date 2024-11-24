---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by 37065.
--- DateTime: 2024/9/16 0:59
---

local Screen = require "Screen"
local M = UnLua.Class()

Screen.Print(string.format("Self Is:%s", M))
function M.SayHi(name)
    local Msg = name .. "\n\n" ..
            [[这是从CPP调用Lua的问候。 —— 本示例来自 "CPPCallLua.lua"]]
    
    Screen.Print(Msg)
    return 1;
end

function M.OtherSayHi(name)
    local Msg = name .. "\n\n" ..
            [[这是从CPP调用Lua的OtherSayHi。测试同一个Env复用。—— 本示例来自 "CPPCallLua.lua"]]
    Screen.Print(Msg)
    M:TestSelf()
    
    return 1;
end

function M:TestSelf()
    Screen.Print(string.format("TestSelf:%s", self))

    return 1;
end

return M