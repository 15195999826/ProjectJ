---
--- Generated by Code, 不要修改函数和入参
--- Created by Lomo.
--- DateTime: 2024-12-15
--- RowName：巨石
---

local M = {StartTickFrame = -1}




---[[[
--- 每帧执行
---@param OwnerID integer
---@param Frame integer
---@param ExecHelper UProjectJCardExecuteHelper
---@return boolean
---]]]
function M:ExecuteTick(OwnerID, Frame, ExecHelper)
    return false;
end


---[[[
--- 隐藏动画播放结束后执行
---@param OwnerID integer
---@param ExecHelper UProjectJCardExecuteHelper
---]]]
function M:ExecuteAfterHide(OwnerID, ExecHelper)
end

return M