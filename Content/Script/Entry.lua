---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by 37065.
--- DateTime: 2024/9/15 14:53
---

local M = UnLua.Class()

require("ProjectJGameplayTags")
require("ProjectJGlobal")
require("TableRows")

---Unlua本地化逻辑： lua脚本中使用LOCTEXT("key", "value")来编写代码， Editor中实现功能：读取lua脚本中的LOCTEXT，将Key和Value写入Config文件中
_G.GetLuaLocText = UE.UProjectJBlueprintFunctionLibrary.GetLocText
_G.NSLOCTEXT = function(InNameSpace, InKey, InValue)
    return GetLuaLocText(InNameSpace, InKey, InValue)
end
_G.newInstance = function(Class)
    local C = require(Class);
    o = {};
    setmetatable(o, C);
    C.__index = C;
    return o;
end

--- 这里打印不对是因为初始化时机过早了， 实际上这个方案是可用的
---local T = NSLOCTEXT("Unlua", "Hello", "你好世界");
---print(T)
function InitDebug()
    package.cpath = package.cpath .. ';C:/Users/Administrator/AppData/Roaming/JetBrains/Rider2024.1/plugins/EmmyLua/debugger/emmy/windows/x64/?.dll'
    local dbg = require('emmy_core')
    dbg.tcpConnect('localhost', 9966)
end

local socket = require("socket")

function isPortInUse(port)
    local tcp = assert(socket.tcp())
    local result = tcp:bind("127.0.0.1", port)
    if result == nil then
        return true
    else
        tcp:close()
        return false
    end
end

-- 检查端口9966是否被占用
local port = 9966
if isPortInUse(port) then
    print("Port " .. port .. " is in use.")
    -- 尝试连接调试器， 连接失败打印日志
    local status, err = pcall(InitDebug)
    if not status then
        print("Connect to EmmyLua Debug failed: " .. err)
    else
        print("Connect to EmmyLua Debug success")
    end
    
else
    print("Port " .. port .. " is available.")
end

--- 测试十次 随机数
--- integer 的随机是 [1,2]
--for i = 1, 10 do
--    print(math.random(1, 2))
--end

print("Hello UnLua Entry")

---- Collect garbage to get an accurate measurement
--collectgarbage("collect")
--
---- Get the current memory usage in kilobytes
--local memoryUsage = collectgarbage("count")
--
---- Print the memory usage
--print("Current memory usage: " .. memoryUsage .. " KB")

return M