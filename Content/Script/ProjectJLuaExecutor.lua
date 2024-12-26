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

---@type UProjectJCardExecuteHelper
_G.ExecHelper = nil;

---@param CharacterRow string
---@param ConditionTable table
---@return integer
_G.SpawnCharacter = function(CharacterRow, ConditionTable)
    local NewCharacterID = ExecHelper:SpawnCharacter(CharacterRow);
    if ConditionTable ~= nil then
        --- Todo: 设置刷新条件
    end
    return NewCharacterID;
end

---@param LandmarkRow string
---@param ConditionTable table
---@return integer
_G.SpawnLandmark = function(LandmarkRow, ConditionTable)
    local NewLandmarkID = ExecHelper:SpawnLandmark(LandmarkRow);
    if ConditionTable ~= nil then

    end
    return NewLandmarkID;
end

---@param UtilityRow string
---@param ConditionTable table
---@return integer
_G.SpawnUtility = function(UtilityRow, ConditionTable)
    local NewUtilityID = ExecHelper:SpawnUtility(UtilityRow);
    if ConditionTable ~= nil then

    end
    return NewUtilityID;
end

---@param ItemRow string
---@param ItemType EProjectJItemType
---@param ConditionTable table
---@return integer
_G.SpawnItem = function(ItemRow, ItemType, ConditionTable)
    local NewItemID = ExecHelper:SpawnItem(ItemType, ItemRow);
    if ConditionTable ~= nil then

    end
    return NewItemID;
end


--- 只提供相关的功能函数，不保存任何状态， 状态都在UE侧保存
local DungeonInstance = {}
local CharacterInstance = {}
local LandmarkInstance = {}
local UtilityInstance = {}
local ItemInstance = {}
local SpellInstance = {}
local EventInstances = {}

local function GetExecInstance(CardType, ID)
    if CardType == UE.EProjectJCardType.Character then
        return CharacterInstance[ID];
    elseif CardType == UE.EProjectJCardType.Landmark then
        return LandmarkInstance[ID];
    elseif CardType == UE.EProjectJCardType.Item then
        return ItemInstance[ID];
    elseif CardType == UE.EProjectJCardType.Utility then
        return UtilityInstance[ID];
    elseif CardType == UE.EProjectJCardType.Spell then
        return SpellInstance[ID];
    end

    return nil;
end


_G.EmptyTagArray = UE.TArray(UE.FGameplayTag());

function M:ReceiveBeginPlay()
    ExecHelper = UE.USubsystemBlueprintLibrary.GetWorldSubsystem(self, UE.UProjectJCardExecuteHelper);
end

---@param RowName string
---@param InLuaScriptName string
function M:CreateDungeon(RowName, InLuaScriptName) 
    if DungeonInstance[RowName] ~= nil then
        Screen.Print("创建副本脚本失败,已经存在副本：" .. RowName);
    end

    DungeonInstance[RowName] = newInstance("Dungeons." .. InLuaScriptName);
end


---@param RowName string
function M:EnterDungeon(RowName)
    local EventSystem = UE.USubsystemBlueprintLibrary.GetWorldSubsystem(self, UE.UProjectJEventSystem);
    DungeonInstance[RowName]:EnterDungeon(EventSystem);
end

---@param RowName string
function M:RemoveDungeon(RowName) 
    DungeonInstance[RowName] = nil;
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
    CharacterInstance[ID].SelfID = ID;
end

---@param ID integer
---@param InLuaScriptName string
function M:CreateLandMark(ID, InLuaScriptName) 
    if LandmarkInstance[ID] ~= nil then
        Screen.Print("创建地标脚本失败,已经存在地标：" .. ID);
    end
    
    LandmarkInstance[ID] = newInstance("Landmarks." .. InLuaScriptName);
    LandmarkInstance[ID].SelfID = ID;
end

---@param ID integer
---@param InLuaScriptName string
function M:CreateUtility(ID, InLuaScriptName)
    if UtilityInstance[ID] ~= nil then
        Screen.Print("创建Utility脚本失败,已经存在Utility：" .. ID);
    end

    UtilityInstance[ID] = newInstance("Utilities." .. InLuaScriptName);
    UtilityInstance[ID].SelfID = ID;
end

---@param ID integer
---@param InLuaScriptName string
function M:CreateItem(ID, InLuaScriptName)
    if ItemInstance[ID] ~= nil then
        Screen.Print("创建物品脚本失败,已经存在物品：" .. ID);
    end

    ItemInstance[ID] = newInstance("Items." .. InLuaScriptName);
    ItemInstance[ID].SelfID = ID;
end


---@param ID integer
---@param InLuaScriptName string
function M:CreateSpell(ID, InLuaScriptName) 
    if SpellInstance[ID] ~= nil then
        Screen.Print("创建行为脚本失败,已经存在行为：" .. ID);
    end

    SpellInstance[ID] = newInstance("Spells." .. InLuaScriptName);
    SpellInstance[ID].SelfID = ID;
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
function M:RemoveUtility(ID) 
    if UtilityInstance[ID] == nil then
        Screen.Print("删除Utility脚本失败,不存在Utility：" .. ID);
    end

    UtilityInstance[ID] = nil;
end

---@param ID integer
function M:RemoveItem(ID)
    if ItemInstance[ID] == nil then
        Screen.Print("删除物品脚本失败,不存在物品：" .. ID);
    end

    ItemInstance[ID] = nil;
end

---@param ID integer
function M:RemoveSpell(ID)
    if SpellInstance[ID] == nil then
        Screen.Print("删除行为脚本失败,不存在行为：" .. ID);
    end

    SpellInstance[ID] = nil;
end


---@param ID integer
---@param InCardType EProjectJCardType
---@return FProjectJTargetFilter
function M:GetTargetFilter(ID, InCardType) 
    local Instance = GetExecInstance(InCardType, ID);
    if Instance == nil then
        return UE.FProjectJTargetFilter();
    end

    return Instance:GetTargetFilter();
end

---@param CardType EProjectJCardType
---@param ID integer
---@param SelectedCardType EProjectJCardType
---@param SelectedID integer
function M:ExecuteSelectTarget(CardType, ID, SelectedCardType, SelectedID)
    local Instance = GetExecInstance(CardType, ID);
    if Instance == nil then
        Screen.Print("执行脚本失败,不存在Type:" .. CardType .. " ID:" .. ID);
        return;
    end

    Instance.SelectedCardType = SelectedCardType;
    Instance.SelectedID = SelectedID;
    Instance:ExecuteSelectTarget();
end

---@param CardType EProjectJCardType
---@param ID integer
---@return integer
function M:GetExecuteMinutes(CardType, ID) 
    local Instance = GetExecInstance(CardType, ID);
    if Instance == nil then
        Screen.Print("执行脚本失败,不存在Type:" .. CardType .. " ID:" .. ID);
        return 5;
    end
    
    return Instance:GetExecuteMinutes();
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
function M:ExecuteTick(CardType, ID, LogicFrame)
    local Instance = GetExecInstance(CardType, ID);
    if Instance == nil then
        Screen.Print("执行脚本失败,不存在Type:" .. CardType .. " ID:" .. ID);
        return;
    end
    Instance:ExecuteTick(LogicFrame);
end

---@param CardType EProjectJCardType
---@param ID integer
function M:ExecuteOver(CardType, ID) 
    local Instance = GetExecInstance(CardType, ID);
    if Instance == nil then
        Screen.Print("执行脚本失败,不存在Type:" .. CardType .. " ID:" .. ID);
        return;
    end
    Instance:ExecuteOver();
end

---@param CardType EProjectJCardType
---@param ID integer
function M:ExecuteAfterHide(CardType, ID) 
    local Instance = GetExecInstance(CardType, ID);
    if Instance == nil then
        Screen.Print("执行脚本失败,不存在Type:" .. CardType .. " ID:" .. ID);
        return;
    end
    Instance:ExecuteAfterHide(ID);
end

---@param SpellTag string
---@param CardType EProjectJCardType
---@param ID integer
---@param RollResult integer
function M:OnGetSpellResult(SpellTag, CardType, ID, RollResult)
    local Instance = GetExecInstance(CardType, ID);
    if Instance == nil then
        Screen.Print("执行脚本失败,不存在Type:" .. CardType .. " ID:" .. ID);
        return;
    end
    
    Instance:OnGetSpellResult(SpellTag, RollResult);
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
