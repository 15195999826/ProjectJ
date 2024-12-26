---
--- Generated by Code, 不要修改函数和入参
--- Created by Lomo.
--- DateTime: 2024-11-19
--- RowName：岛主
---

local M = {SelfID = nil, StartTickFrame = nil}




---[[[
--- 每帧执行
---@param Frame integer
---]]]
function M:ExecuteTick(Frame)
    return false;
end


---[[[
--- 隐藏动画播放结束后执行
---]]]
function M:ExecuteAfterHide()
end


---[[[
--- 返回执行时间(分钟)
--- @return integer
---]]]
function M:GetExecuteMinutes()
    return 15;
end


---[[[
--- 执行完毕
---]]]
function M:ExecuteOver()

end


---[[[
--- 收到了行为卡的执行结果
--- @param SpellTag string
--- @param RollResult integer
---]]]
function M:OnGetSpellResult(SpellTag, RollResult)
    ExecHelper:DefaultActionToSpell(SpellTag, self.SelfID);
end

return M