--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type BP_LuaExecutor_C
local M = UnLua.Class()

--- 只提供相关的功能函数，不保存任何状态， 状态都在UE侧保存
local LevelInstance = {}
local CharacterInstance = {}
local LandmarkInstance = {}
local EventInstances = {}

_G.EmptyTagArray = UE.TArray(UE.FGameplayTag());

--function M:ReceiveBeginPlay()
--    local T = UE.UProjectJBlueprintFunctionLibrary.GetLocText("Hello", "你好世界")
--    print(T)
--    T = NSLOCTEXT("Unlua", "Hello", "你好世界")
--    print(T)
--end

---@param RowName string
---@param ScriptName string
function M:FirstTimeEnterLevel(RowName, ScriptName)
    if LevelInstance[RowName] == nil then
        LevelInstance[RowName] = newInstance("Levels." .. ScriptName);
    end

    local ContextSystem = UE.USubsystemBlueprintLibrary.GetWorldSubsystem(self, UE.UProjectJContextSystem);
    local EventSystem = UE.USubsystemBlueprintLibrary.GetWorldSubsystem(self, UE.UProjectJEventSystem);
    LevelInstance[RowName]:FirstTimeEnterLevel(ContextSystem, EventSystem);
end

---@param RowName string
function M:EnterLevel(RowName)
    local ContextSystem = UE.USubsystemBlueprintLibrary.GetWorldSubsystem(self, UE.UProjectJContextSystem);
    local EventSystem = UE.USubsystemBlueprintLibrary.GetWorldSubsystem(self, UE.UProjectJEventSystem);
    LevelInstance[RowName]:EnterLevel(ContextSystem, EventSystem);
end


---@param InLuaScriptName string
---@return string
function M:GetLuaAbilityDesc(InLuaScriptName) 
    local Module = require("Abilities." .. InLuaScriptName);
    return Module:GetDesc();
end

---@param InOwnerID integer
---@param InEventID integer
---@param InLuaScriptName string
function M:CreateLuaAbilityInstance(InOwnerID, InEventID, InLuaScriptName)
    local ModulePath = "Abilities." .. InLuaScriptName;
    local NewInstance = newInstance(ModulePath);
    EventInstances[InOwnerID] = EventInstances[InOwnerID] or {};
    EventInstances[InOwnerID][InEventID] = NewInstance;
end

---@param InOwnerID integer
---@param EventID integer
---@return FGameplayTag
function M:GetExecTag(InOwnerID, EventID)
    local Instance = EventInstances[InOwnerID][EventID]
    if Instance == nil then
        return UE.FGameplayTag();
    end

    return Instance:GetExecTag();
end

---@param InOwnerID integer
---@param EventID integer
---@param ProjectJEventData FProjectJBattleEventData
---@return boolean
function M:IsTriggerTime(InOwnerID, EventID, ProjectJEventData) 
    local Instance = EventInstances[InOwnerID][EventID];
    if Instance == nil then
        return false;
    end
    ---@type UProjectJContextSystem
    local ContextSystem = UE.USubsystemBlueprintLibrary.GetWorldSubsystem(self, UE.UProjectJContextSystem);
    ---@type AProjectJBattleManager
    local BattleManager = ContextSystem.BattleManager;
    return Instance:IsTriggerTime(InOwnerID, ProjectJEventData, BattleManager);
end

---@param InOwnerID integer
---@param EventID integer
---@param ProjectJEventData FProjectJBattleEventData
---@return FProjectJLuaAbilityExecInfo
function M:GetLuaAbilityExecInfo(InOwnerID, EventID, ProjectJEventData)
    local Instance = EventInstances[InOwnerID][EventID];
    if Instance == nil then
        return UE.FProjectJLuaAbilityExecInfo();
    end
    
    ---@type UProjectJContextSystem
    local ContextSystem = UE.USubsystemBlueprintLibrary.GetWorldSubsystem(self, UE.UProjectJContextSystem);
    ---@type AProjectJBattleManager
    local BattleManager = ContextSystem.BattleManager;
    return Instance:GetLuaAbilityExecInfo(InOwnerID, ProjectJEventData, BattleManager);
end

---获取松散特性标签, 用于一些特殊功能实现， 只是简单的给角色添加松散Tag
---Todo: 是否需要考虑层数的问题
---@param InOwnerID integer
---@param EventID integer
---@return TArray<FGameplayTag>
function M:GetLooseTag(InOwnerID, EventID)
    local Instance = EventInstances[InOwnerID][EventID];
    if Instance == nil then
        return EmptyTagArray;
    end

    return Instance:LooseTagArray();
end


---@param InOwnerID integer
---@param EventID integer
---@param ProjectJEventData FProjectJBattleEventData
---@param InTargets TArray<integer>
function M:ExecuteLuaAbility(InOwnerID, EventID, ProjectJEventData, InTargets)
    local Instance = EventInstances[InOwnerID][EventID];
    if Instance == nil then
        print(InOwnerID, EventID, "Instance is nil");
        return;
    end

    ---@type UProjectJContextSystem
    local ContextSystem = UE.USubsystemBlueprintLibrary.GetWorldSubsystem(self, UE.UProjectJContextSystem);
    ---@type AProjectJBattleManager
    local BattleManager = ContextSystem.BattleManager;
    Instance:ExecuteLuaAbility(InOwnerID, ProjectJEventData, InTargets, BattleManager);
end

---@param InOwnerID integer
---@param EventID integer
function M:RemoveLuaAbilityInstance(InOwnerID, EventID)
    EventInstances[InOwnerID][EventID] = nil
end

return M
