---
--- Generated by Code, 不要修改函数和入参
--- Created by Lomo.
--- DateTime: 2024-12-23
--- RowName：石头
---

local M = {SelfID = nil, StartTickFrame = nil, SelectedID = nil, SelectedCardType = nil}

---[[[
--- 返回可以使用的目标筛选器； 默认对所有角色、地标可用
--- @return FProjectJTargetFilter
---]]]
function M:GetTargetFilter()
   return DefaultPropTargetFilter;
end

---[[[
--- 选择目标时执行
---]]]
function M:ExecuteSelectTarget()

end

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
--- 选择目标时执行
---]]]
function M:ExecuteSelectTarget()

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