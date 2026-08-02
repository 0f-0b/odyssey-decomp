#pragma once

#include "Library/Event/IEventFlowEventReceiver.h"
#include "Library/Event/IEventFlowQueryJudge.h"
#include "Library/LiveActor/LiveActor.h"

class Doshi;

class ShoppingWatcher : public al::LiveActor,
                        public al::IEventFlowEventReceiver,
                        public al::IEventFlowQueryJudge {
public:
    ShoppingWatcher(const char* name, const char* = nullptr, Doshi* = nullptr);

    void init(const al::ActorInitInfo& info) override;
    bool isAfterBuyWear() const;
    bool isAfterBuyShine() const;
    bool isAfterBuyLifeUpItem() const;
    void initAfterPlacement() override;
    bool tryStartCameraAfterBuyItem();
    void control() override;
    void attackSensor(al::HitSensor* self, al::HitSensor* other) override;
    bool receiveMsg(const al::SensorMsg* message, al::HitSensor* other,
                    al::HitSensor* self) override;
    bool receiveEvent(const al::EventFlowEventData* event) override;
    const char* judgeQuery(const char* judge_name) const override;

    void exeWait();
    void exeScare();
    void exeShop();
    void exeDemoWear();
    bool tryEndCameraAfterBuyItem();
    void exeDemoGetShine();
    void exeDemoGetLifeUpItem();
    void exeEnd();
    void exeReaction();
    void exeTurnToInitFront();
    void exeTimeBalloonOrRace();

    bool isAliveNpcInShop() const;
    bool isWait() const;
    bool isShop() const;
    void endShop();
    void appearNpcInShop();
    void killNpcInShop();
    const sead::Vector3f& getDoshiPos();
    void calcDoshiFrontDir(sead::Vector3f* out);
    bool isInsideTerritoryPlayer() const;
    void requestStartDoshiCamera();
    void requestEndDoshiCamera();

private:
    void* _padding[0x2e];
};

static_assert(sizeof(ShoppingWatcher) == 0x288);
