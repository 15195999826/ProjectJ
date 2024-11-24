---
--- Generated by Code, 不要修改函数和入参
--- Created by Lomo.
--- DateTime: 2024-11-19
--- RowName：中心广场
---

local M = {}

--- 首次进入该关卡时
---[[[
---@param ContextSystem UProjectJContextSystem
---@param EventSystem UProjectJEventSystem
---]]]
function M:FirstTimeEnterLevel(ContextSystem, EventSystem)
    --- 播放剧情， 写入剧情描述
    local StoryRecord = UE.FProjectJStoryRecord();
    StoryRecord.Story:Add(
            NSLOCTEXT("Unlua", "Story_ZhongXinGuangChang_1", "夜色已深，到了熄灯安睡的时间，桃花岛却响起了紧急集合的钟声。听到钟声，所有人点起灯笼，披上衣服，陆陆续续聚集到梅园中心的广场。")
    );
    StoryRecord.Story:Add(
            NSLOCTEXT("Unlua", "Story_ZhongXinGuangChang_2", "广场中心站着东方鸿，他是一只兔妖，也是桃花岛的岛主，他带着几十只小体形的兽族隐居在这个风景如画满是桃花的小岛上，他平时和蔼可亲，但今天看起来格外严肃。")
    );
    StoryRecord.Story:Add(
            NSLOCTEXT("Unlua", "Story_ZhongXinGuangChang_3", "他轻点完人数，沉默了一会，也不做多余解释，只说：从今天起桃花岛封岛，岛上所有兽族不得擅自离岛，也不接待任何前来桃花岛的来客。")
    );
    StoryRecord.Story:Add(
            NSLOCTEXT("Unlua", "Story_ZhongXinGuangChang_4", "你的耳朵动了动，你是一只小兔子精，名叫芍药。从今天岛主的反常行为来看，你觉得一定发生了什么大事，但是岛主不说直接封岛，你陷入了沉思。")
    );
    --- Todo: 设置剧情使用的角色Avatar
    
    --- Todo: 剧情结束后播放的动画， 比如三个地点的出现动画
    
    EventSystem.PostStoryRecord:Broadcast(StoryRecord);
end

--- 进入该关卡时 (PS: 首次进入时先调用FirstTimeEnterLevel再调用EnterLevel)
---[[[
---@param ContextSystem UProjectJContextSystem
-----@param EventSystem UProjectJEventSystem
---]]]
function M:EnterLevel(ContextSystem, EventSystem)
    
end

return M