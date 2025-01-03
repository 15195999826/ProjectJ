---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by Administrator.
--- DateTime: 2024/11/7 18:10
---

local M = {}
--
-----@param OwnerID integer
-----@param Capture FAutoDWBattleStatusRecord
-----@param BattleBackend AAutoDWBattleBackend
-----@return TArray<FAutoDWCharacterCapture>
--function M:GetOpponentTeam(OwnerID, Capture, BattleBackend)
--    local SelfTeamID = BattleBackend:GetTeam(OwnerID, Capture);
--    local OpponentTeam;
--    if SelfTeamID == 0 then
--        OpponentTeam = Capture.RightTeam;
--    else
--        OpponentTeam = Capture.LeftTeam;
--    end
--    
--    return OpponentTeam;
--end
--
-----@param OwnerID integer
-----@param Capture FAutoDWBattleStatusRecord
-----@param BattleBackend AAutoDWBattleBackend
-----@return TArray<FAutoDWCharacterCapture>
--function M:GeTeammateTeam(OwnerID, Capture, BattleBackend)
--    local SelfTeamID = BattleBackend:GetTeam(OwnerID, Capture);
--    local SelfTeam;
--    if SelfTeamID == 0 then
--        SelfTeam = Capture.LeftTeam;
--    else
--        SelfTeam = Capture.RightTeam;
--    end
--
--    return SelfTeam;
--end
--
-----@param RandomCount integer
-----@param OwnerID integer
-----@param Capture FAutoDWBattleStatusRecord
-----@param BattleBackend AAutoDWBattleBackend
-----@return TArray<integer>
--function M:GetRandomEnemies(RandomCount, OwnerID, Capture, BattleBackend)
--    local  TargetTeam = M:GetOpponentTeam(OwnerID, Capture, BattleBackend);
--    --- 随机RandomCount名不同的敌人
--    local SelectedEnemies = UE.TArray(0);
--    local OpponentCount = TargetTeam:Num();
--    if RandomCount > OpponentCount then
--        RandomCount = OpponentCount;
--    end
--    
--    for i = 1, RandomCount do
--        local RandomIndex = math.random(1, OpponentCount);
--        local TargetID = TargetTeam[RandomIndex].ID;
--        --- 检查SelectedEnemies中是否已经有了这个敌人
--        while SelectedEnemies:Contains(TargetID) do
--            RandomIndex = math.random(1, OpponentCount);
--            TargetID = TargetTeam[RandomIndex].ID;
--        end
--        SelectedEnemies:Add(TargetID);
--    end
--    
--    return SelectedEnemies;
--end
--
-----@param RandomCount integer
-----@param OwnerID integer
-----@param Capture FAutoDWBattleStatusRecord
-----@param BattleBackend AAutoDWBattleBackend
-----@param CanSelf boolean
-----@return TArray<integer>
--function M:GetRandomTeammates(RandomCount, OwnerID, Capture, BattleBackend, CanSelf)
--    local  TargetTeam = M:GeTeammateTeam(OwnerID, Capture, BattleBackend);
--    --- 随机RandomCount名不同的队友， 根据CanSelf决定是否包含自己
--    local SelectedTeammates = UE.TArray(0);
--
--    local TeammateCount = TargetTeam:Num();
--    local SelfIsInTargetTeam = false;
--    for i = 1, TeammateCount do
--        if TargetTeam[i].ID == OwnerID then
--            SelfIsInTargetTeam = true;
--            break;
--        end
--    end
--    ---如果不允许包含自己
--    if not CanSelf and SelfIsInTargetTeam then
--        TeammateCount = TeammateCount - 1;
--        ---如果队友数量小于等于0， 直接返回
--        if TeammateCount <= 0 then
--            return SelectedTeammates;
--        end
--    end
--    
--    if RandomCount > TeammateCount then
--        RandomCount = TeammateCount;
--    end
--    
--    for i = 1, RandomCount do
--        local RandomIndex = math.random(1, TargetTeam:Num());
--        local TargetID = TargetTeam[RandomIndex].ID;
--        --- 检查SelectedTeammates中是否已经有了这个队友， 或者 不允许包含自己时，TargetID是自己
--        while SelectedTeammates:Contains(TargetID) or (not CanSelf and TargetID == OwnerID) do
--            RandomIndex = math.random(1, TargetTeam:Num());
--            TargetID = TargetTeam[RandomIndex].ID;
--        end
--        SelectedTeammates:Add(TargetID);
--    end
--    
--    
--    return SelectedTeammates;
--end
--
-----随机指定数量单位，不分敌我
-----@param RandomCount integer
-----@param OwnerID integer
-----@param Capture FAutoDWBattleStatusRecord
-----@param CanSelf boolean 是否允许包含自己
-----@return table
--function M:GetRandomAny(RandomCount, OwnerID, Capture, CanSelf)
--   --- 合并Capture.LeftTeam和Capture.RightTeam
--    local AllCharacterIDTable ={}
--    for i = 1, Capture.LeftTeam:Num() do
--        local ID = Capture.LeftTeam[i].ID;
--        if CanSelf or ID ~= OwnerID then
--            AllCharacterIDTable[#AllCharacterIDTable + 1] = ID;
--        end
--    end
--    
--    for i = 1, Capture.RightTeam:Num() do
--        local ID = Capture.RightTeam[i].ID;
--        if CanSelf or ID ~= OwnerID then
--            AllCharacterIDTable[#AllCharacterIDTable + 1] = ID;
--        end
--    end
--    
--    if RandomCount > #AllCharacterIDTable then
--        --- 直接返回全部
--        return AllCharacterIDTable;
--    end
--
--    local SelectedIDs = {};
--
--    --- 随机不重复的RandomCount个单位
--    for i = 1, RandomCount do
--        local RandomIndex = math.random(1, #AllCharacterIDTable);
--        SelectedIDs[#SelectedIDs + 1] = AllCharacterIDTable[RandomIndex];
--        table.remove(AllCharacterIDTable, RandomIndex);
--    end
--    
--
--    return SelectedIDs;
--end
--
-----@param OwnerID integer
-----@param Capture FAutoDWBattleStatusRecord
-----@param BattleBackend AAutoDWBattleBackend
-----@return integer
--function M:GetBackTeammate(OwnerID, Capture, BattleBackend)
--    local TargetTeam = M:GeTeammateTeam(OwnerID, Capture, BattleBackend);
--    local SelfPosition = BattleBackend:GetPosition(OwnerID, Capture);
--    --- 获取SelfPosition + 1的队友
--    for i = 1, TargetTeam:Num() do
--        local TeammateID = TargetTeam[i].ID;
--        local TargetPosition = BattleBackend:GetPosition(TeammateID, Capture);
--        if TargetPosition == SelfPosition + 1 then
--            return TeammateID;
--        end
--    end
--    
--    return nil;
--end
--
-----@param OwnerID integer
-----@param Capture FAutoDWBattleStatusRecord
-----@param BattleBackend AAutoDWBattleBackend
-----@param IsTeammate boolean
-----@return TArray<integer>
--function M:GetEmptyPosition(OwnerID, Capture, BattleBackend, IsTeammate)
--    local TargetTeam;
--    if IsTeammate then
--        TargetTeam = M:GeTeammateTeam(OwnerID, Capture, BattleBackend);
--    else
--        TargetTeam = M:GetOpponentTeam(OwnerID, Capture, BattleBackend);
--    end
--    
--    local EmptyPosition = UE.TArray(0);
--    EmptyPosition:Add(0);
--    EmptyPosition:Add(1);
--    EmptyPosition:Add(2);
--    EmptyPosition:Add(3);
--    EmptyPosition:Add(4);
--    
--    for i = 1, TargetTeam:Num() do
--        local Character = TargetTeam[i];
--        EmptyPosition:Remove(Character.Position);
--    end
--    
--    return EmptyPosition;
--end
--
-----@param Capture FAutoDWBattleStatusRecord
-----@return table
--function M:GetAllAlive(Capture)
--    --- 合并Capture.LeftTeam和Capture.RightTeam
--    local AllCharacterIDTable ={}
--    for i = 1, Capture.LeftTeam:Num() do
--        local ID = Capture.LeftTeam[i].ID;
--        if CanSelf or ID ~= OwnerID then
--            AllCharacterIDTable[#AllCharacterIDTable + 1] = ID;
--        end
--    end
--
--    for i = 1, Capture.RightTeam:Num() do
--        local ID = Capture.RightTeam[i].ID;
--        if CanSelf or ID ~= OwnerID then
--            AllCharacterIDTable[#AllCharacterIDTable + 1] = ID;
--        end
--    end
--
--    return AllCharacterIDTable;
--end

return M