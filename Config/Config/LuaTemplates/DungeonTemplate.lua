﻿---
--- Generated by Code, 不要修改函数和入参
--- Created by Lomo.
--- DateTime: {DateTime}
--- RowName：{RowName}
---

local M = {StartTickFrame = -1}

---[[[
--- 进入该关卡时
---@param EventSystem UProjectJEventSystem
---]]]
function M:EnterDungeon(EventSystem)
    ---关卡内卡牌:[[[
    
    ---]]]
    
    ---播放剧情， 写入剧情描述
    ---Story: [[[
    local StoryRecord = UE.FProjectJStoryRecord();
    StoryRecord.Story:Add(
            NSLOCTEXT("Unlua", "Story_{LuaScriptName}_1", "默认文本")
    );
    ---]]]

    EventSystem.PostStoryRecord:Broadcast(StoryRecord);
end

return M