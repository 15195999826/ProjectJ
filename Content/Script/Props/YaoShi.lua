---
--- Generated by Code, 不要修改函数和入参
--- Created by Lomo.
--- DateTime: 2024-12-22
--- RowName：钥匙
---

local M = {StartTickFrame = -1}

---[[[
--- 返回可以使用的目标筛选器； 默认对所有角色、地标可用
--- @return FProjectJTargetFilter
---]]]
function M:GetTargetFilter() 
    --- 只能对地标使用
    local Filter = FProjectJTargetFilter();
    Filter.TargetTypes:Add(EProjectJTargetType.Landmark);
    return Filter;
end

---[[[
--- 每帧执行
---@param OwnerID integer
---@param Frame integer
---@param ExecHelper UProjectJCardExecuteHelper
---@return boolean
---]]]
function M:ExecuteTick(OwnerID, Frame, ExecHelper)
    --- 简单表达5帧后执行完成
    if (Frame - self.StartTickFrame) == 5 then
        return true;
    end

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