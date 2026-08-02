#pragma once

#include "Library/Event/EventFlowNode.h"

class EventFlowNodeCheckFlag : public al::EventFlowNode {
public:
    EventFlowNodeCheckFlag(const char* name);
    void init(const al::EventFlowNodeInitInfo& info) override;
    s32 getNextId() const override;

    void start() override { end(); }
};

class EventFlowNodeCheckOpenDoorSnow : public al::EventFlowNode {
public:
    EventFlowNodeCheckOpenDoorSnow(const char* name);
    void init(const al::EventFlowNodeInitInfo& info) override;
    s32 getNextId() const override;

    void start() override { end(); }
};

class EventFlowNodeCheckPlayingCollectBgm : public al::EventFlowNode {
public:
    EventFlowNodeCheckPlayingCollectBgm(const char* name);
    void init(const al::EventFlowNodeInitInfo& info) override;
    s32 getNextId() const override;

    void start() override { end(); }

private:
    const char* mRequest = nullptr;
};

class EventFlowNodeCheckFirstTalkCollectBgm : public al::EventFlowNode {
public:
    EventFlowNodeCheckFirstTalkCollectBgm(const char* name);
    void init(const al::EventFlowNodeInitInfo& info) override;
    s32 getNextId() const override;

    void start() override { end(); }
};

class EventFlowNodeFirstTalkEndCollectBgmNpc : public al::EventFlowNode {
public:
    EventFlowNodeFirstTalkEndCollectBgmNpc(const char* name);
    void init(const al::EventFlowNodeInitInfo& info) override;
    void start() override;
};

class EventFlowNodeCheckClear3CollectBgm : public al::EventFlowNode {
public:
    EventFlowNodeCheckClear3CollectBgm(const char* name);
    void init(const al::EventFlowNodeInitInfo& info) override;
    s32 getNextId() const override;

    void start() override { end(); }
};

class EventFlowNodeCheckCompleteCollectBgm : public al::EventFlowNode {
public:
    EventFlowNodeCheckCompleteCollectBgm(const char* name);
    void init(const al::EventFlowNodeInitInfo& info) override;
    s32 getNextId() const override;

    void start() override { end(); }
};

class EventFlowNodeGetCollectBgmBonus01 : public al::EventFlowNode {
public:
    EventFlowNodeGetCollectBgmBonus01(const char* name);
    void init(const al::EventFlowNodeInitInfo& info) override;
    void start() override;
};

class EventFlowNodeGetCollectBgmBonus02 : public al::EventFlowNode {
public:
    EventFlowNodeGetCollectBgmBonus02(const char* name);
    void init(const al::EventFlowNodeInitInfo& info) override;
    void start() override;
};
