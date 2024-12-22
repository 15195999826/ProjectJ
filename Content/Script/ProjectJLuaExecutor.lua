--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

---@type BP_LuaExecutor_C
local M = UnLua.Class()
local Screen = require "Screen"

--- 只提供相关的功能函数，不保存任何状态， 状态都在UE侧保存
local LevelInstance = {}
local CharacterInstance = {}
local LandmarkInstance = {}
local ItemInstance = {}
local EventInstances = {}

local function GetExecInstance(CardType, ID)
    if CardType == UE.EProjectJCardType.Character then
        return CharacterInstance[ID];
    elseif CardType == UE.EProjectJCardType.Landmark then
        return LandmarkInstance[ID];
    elseif CardType == UE.EProjectJCardType.Item then
        return ItemInstance[ID];
    end

    return nil;
end


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

---@return boolean
function M:CanExecute() 
    return true;
end

---@param ID integer
---@param InLuaScriptName string
function M:CreateCharacter(ID, InLuaScriptName) 
    if CharacterInstance[ID] ~= nil then
        Screen.Print("创建角色脚本失败,已经存在角色：" .. ID);
    end

    CharacterInstance[ID] = newInstance("Characters." .. InLuaScriptName);
end

---@param ID integer
---@param InLuaScriptName string
function M:CreateLandMark(ID, InLuaScriptName) 
    if LandmarkInstance[ID] ~= nil then
        Screen.Print("创建地标脚本失败,已经存在地标：" .. ID);
    end
    
    LandmarkInstance[ID] = newInstance("Landmarks." .. InLuaScriptName);
end

---@param ID integer
---@param InLuaScriptName string
function M:CreateItem(ID, InLuaScriptName)
    if ItemInstance[ID] ~= nil then
        Screen.Print("创建物品脚本失败,已经存在物品：" .. ID);
    end

    ItemInstance[ID] = newInstance("Items." .. InLuaScriptName);
end

---@param ID integer
function M:RemoveCharacter(ID)
    if CharacterInstance[ID] == nil then
        Screen.Print("删除角色脚本失败,不存在角色：" .. ID);
    end
    
    CharacterInstance[ID] = nil;
end

---@param ID integer
function M:RemoveLandMark(ID)
    if LandmarkInstance[ID] == nil then
        Screen.Print("删除地标脚本失败,不存在地标：" .. ID);
    end

    LandmarkInstance[ID] = nil;
end

---@param ID integer
function M:RemoveItem(ID)
    if ItemInstance[ID] == nil then
        Screen.Print("删除物品脚本失败,不存在物品：" .. ID);
    end

    ItemInstance[ID] = nil;
end


---@param ID integer
---@return FProjectJTargetFilter
function M:GetTargetFilter(ID)
    ---目前只有Item需要选择目标
    local Instance = ItemInstance[ID];
    if Instance == nil then
        return UE.FProjectJTargetFilter();
    end
    
    return Instance:GetTargetFilter();
end

---@param CardType EProjectJCardType
---@param ID integer
---@param Frame integer
function M:ExecuteStart(CardType, ID, Frame)
    local Instance = GetExecInstance(CardType, ID);
    if Instance == nil then
        Screen.Print("执行脚本失败,不存在Type:" .. CardType .. " ID:" .. ID);
        return;
    end

    Instance.StartTickFrame = Frame;
end


---@param CardType EProjectJCardType
---@param ID integer
---@param LogicFrame integer
---@return boolean
function M:ExecuteTick(CardType, ID, LogicFrame)
    local Instance = GetExecInstance(CardType, ID);
    if Instance == nil then
        Screen.Print("执行脚本失败,不存在Type:" .. CardType .. " ID:" .. ID);
        return;
    end
    local ExecHelper = UE.USubsystemBlueprintLibrary.GetWorldSubsystem(self, UE.UProjectJCardExecuteHelper);
    return Instance:ExecuteTick(ID, LogicFrame, ExecHelper);
end

---@param CardType EProjectJCardType
---@param ID integer
function M:ExecuteAfterHide(CardType, ID) 
    local Instance = GetExecInstance(CardType, ID);
    if Instance == nil then
        Screen.Print("执行脚本失败,不存在Type:" .. CardType .. " ID:" .. ID);
        return;
    end
    local ExecHelper = UE.USubsystemBlueprintLibrary.GetWorldSubsystem(self, UE.UProjectJCardExecuteHelper);
    Instance:ExecuteAfterHide(ID, ExecHelper);
end
---@param InLuaScriptName string
---@return string
function M:GetLuaAbilityDesc(InLuaScriptName) 
    local Module = require("Abilities." .. InLuaScriptName);
    print("GetModule")
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
