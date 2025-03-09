#pragma once

#include "Library/Collision/MtxConnector.h"

namespace al {

class CollisionPartsConnector : public MtxConnector {
public:
    CollisionPartsConnector();
    void init(const sead::Matrix34f*, const sead::Matrix34f&, const al::CollisionParts*);
    void getConnectingSensor() const;
    void isConnecting() const override;
    virtual void isMoved() const;
    void isConnectInvalidCollision() const;
    void clear() override;

private:
    s64 _0x60 = 0;
};

static_assert(sizeof(CollisionPartsConnector) == 0x68);

}  // namespace al
