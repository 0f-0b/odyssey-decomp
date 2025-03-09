#pragma once

#include <math/seadMatrix.h>
#include <math/seadQuat.h>
#include <math/seadVector.h>

namespace al {
class ActorInitInfo;
class CollisionParts;
class LiveActor;

class MtxConnector {
public:
    MtxConnector();
    MtxConnector(const sead::Quatf&, const sead::Vector3f&);
    void init(const sead::Matrix34f*, const sead::Matrix34f&);
    void init(const sead::Matrix34f*);
    virtual void clear();
    void multVec(sead::Vector3f*, const sead::Vector3f&) const;
    void multTrans(sead::Vector3f*, const sead::Vector3f&) const;
    void multMtx(sead::Matrix34f*, const sead::Matrix34f&) const;
    void multQT(sead::Quatf*, sead::Vector3f*, sead::Vector3f*) const;
    void multQT(sead::Quatf*, sead::Vector3f*, sead::Vector3f*, const sead::Quatf&,
                const sead::Vector3f&) const;
    void multQT(sead::Quatf*, sead::Vector3f*, const sead::Quatf&, const sead::Vector3f&) const;
    void getBaseQuat() const;
    void getBaseTrans() const;
    void setBaseQuatTrans(const sead::Quatf&, const sead::Vector3f&);
    virtual void isConnecting() const;
    void calcConnectInfo(sead::Vector3f*, sead::Quatf*, sead::Vector3f*, const sead::Vector3f&,
                         const sead::Vector3f&) const;
    void calcMtxWithOffset(sead::Matrix34f*, const sead::Vector3f&, const sead::Vector3f&) const;
    void tryGetParentTrans(sead::Vector3f*) const;

private:
    sead::Matrix34f _0x8 = sead::Matrix34f::ident;
    s64 _0x38 = 0;
    sead::Quatf _0x40 = sead::Quatf::unit;
    sead::Vector3f _0x50 = sead::Vector3f::zero;
};

}  // namespace al
