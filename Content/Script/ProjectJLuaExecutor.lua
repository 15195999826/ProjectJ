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


return M
