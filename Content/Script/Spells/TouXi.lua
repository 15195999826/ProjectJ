---
--- Generated by Code, 不要修改函数和入参
--- Created by Lomo.
--- DateTime: 2024-12-25
--- RowName：偷袭
---

local M = {StartTickFrame = -1, SelectedID = nil, SelectedCardType = nil}

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
---@param OwnerID integer
---@param TargetID integer
---@param Frame integer
---@return boolean
---]]]
function M:ExecuteTick(OwnerID, TargetID,Frame)
    return false;
end

---[[[
--- 隐藏动画播放结束后执行
---@param OwnerID integer
---]]]
function M:ExecuteAfterHide(OwnerID)
end


---[[[
--- 选择目标时执行
---]]]
function M:ExecuteSelectTarget()
    
end

return M