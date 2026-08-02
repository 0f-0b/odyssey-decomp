#include "MapObj/LavaWave.h"

#include "Library/Effect/EffectSystemInfo.h"
#include "Library/Effect/PartsEffect.h"
#include "Library/Effect/PartsEffectGroup.h"
#include "Library/Fluid/FluidSimulateWave.h"
#include "Library/Fluid/HeightMapTextureBlender.h"
#include "Library/Fluid/SineWave.h"
#include "Library/LiveActor/ActorInitInfo.h"
#include "Library/LiveActor/ActorInitUtil.h"
#include "Library/LiveActor/ActorPoseUtil.h"
#include "Library/LiveActor/ActorResourceFunction.h"
#include "Library/Math/MathUtil.h"
#include "Library/Nature/NatureUtil.h"
#include "Library/Placement/PlacementFunction.h"
#include "Library/Texture/TextureReplacer.h"
#include "Library/Texture/TextureUtil.h"
#include "Library/Yaml/ByamlIter.h"

LavaWave::LavaWave(const char* name) : al::LiveActor(name) {}

void LavaWave::init(const al::ActorInitInfo& info) {
    al::initMapPartsActor(this, info, nullptr);
    al::registerFluidSurfaceObj(this, this);

    const char* material_name = nullptr;
    const char* height_texture_name = nullptr;
    const char* normal_texture_name = nullptr;
    {
        al::ByamlIter iter{al::getModelResourceYaml(this, "InitWaveInfo", nullptr)};

        for (s32 i = 0, len = iter.getSize(); i < len; i++) {
            al::ByamlIter subiter;
            if (!iter.tryGetIterByIndex(&subiter, i))
                continue;
            subiter.tryGetFloatByKey(&mRadius, "Radius");
            subiter.tryGetIntByKey(&mTextureSize, "TextureSize");
            subiter.tryGetIntByKey(&mNrmTextureSize, "NrmTextureSize");
            subiter.tryGetStringByKey(&material_name, "MaterialName");
            subiter.tryGetStringByKey(&height_texture_name, "HeightTextureName");
            subiter.tryGetStringByKey(&normal_texture_name, "NormalTextureName");
            subiter.tryGetIntByKey(&mBossMagmaEndDemoRipple1st, "BossMagmaEndDemoRipple1st");
            subiter.tryGetIntByKey(&mBossMagmaEndDemoRipple2nd, "BossMagmaEndDemoRipple2nd");
        }
    }

    mSineWave =
        new al::SineWave(info.actorSceneInfo.graphicsSystemInfo, {mTextureSize, mTextureSize},
                         {mRadius + mRadius, mRadius + mRadius});
    mSineWave->set_0x98();
    mFluidSimulateWave =
        new al::FluidSimulateWave(info.actorSceneInfo.graphicsSystemInfo, mTextureSize,
                                  mRadius + mRadius, true, false, 0, "溶岩波");
    mHeightMapTextureBlender = new al::HeightMapTextureBlender(
        info.actorSceneInfo.graphicsSystemInfo, {mTextureSize, mTextureSize},
        {mNrmTextureSize, mNrmTextureSize}, {mRadius + mRadius, mRadius + mRadius});

    mHeightTextureReplacer = new al::TextureReplacer(mSineWave->get_0x40());
    mNormalTextureReplacer = new al::TextureReplacer(mHeightMapTextureBlender->get_0x48());
    mHeightTextureReplacer->replace(this, material_name, height_texture_name);
    mNormalTextureReplacer->replace(this, material_name, normal_texture_name);

    mWaveParams = {200.0f, 0.2f, 10.0f, 45.0f};
    al::tryGetArg(&mWaveParams.x, info, "WaveHeight");
    al::tryGetArg(&mWaveParams.y, info, "WaveWidth");
    al::tryGetArg(&mWaveParams.z, info, "WaveSpeed");
    al::tryGetArg(&mWaveParams.w, info, "WaveAngle");
    al::tryGetArg(&mSimulateWaveViscosity, info, "SimulateWaveViscosity");

    mEffectPos = new sead::Vector3f[20];
    mEffectTimer = new s32[20];
    mEffectScale = new f32[20];
    mEffectGroup = al::createPartsEffectGroup(this, info, 20);
    for (s32 i = 0; i < 20; i++)
        al::setEffectFollowPosPtr(mEffectGroup->getEffect(i), "Boil", &mEffectPos[i]);

    makeActorAlive();
}

void LavaWave::initAfterPlacement() {
    mSineWave->set_0x9a();
    mFluidSimulateWave->set_0x3a1();
    mHeightMapTextureBlender->set_0x8d();
}

void LavaWave::control() {
    mSineWave->set_0x9a();
    mFluidSimulateWave->set_0x3a1();
    mHeightMapTextureBlender->set_0x8d();

    mSineWave->setWaveParams(mWaveParams.x, mWaveParams.y, mWaveParams.z, mWaveParams.w);
    mFluidSimulateWave->set_0x340(_0x154, mSimulateWaveViscosity, _0x15c);

    if (mBossMagmaEndDemoParam > 0)
        return;

    if (mEmitEffectTimer++ >= mEmitEffectInterval) {
        const sead::Vector3f& trans = al::getTrans(this);
        sead::Vector3f pos = {al::getRandom(trans.x - mRadius, trans.x + mRadius), trans.y,
                              al::getRandom(trans.z - mRadius, trans.z + mRadius)};
        sead::Vector3f unused;
        sead::Vector3f surface_pos;

        if (al::calcFindFireSurface(&surface_pos, &unused, this, pos, sead::Vector3f::ey, 3000)) {
            mEffectPos[mEffectIndex].set(surface_pos);
            mEffectTimer[mEffectIndex] = 85;
            al::emitEffect(mEffectGroup->getEffect(mEffectIndex), "Boil", nullptr);
            al::setEffectFollowPosPtr(mEffectGroup->getEffect(mEffectIndex), "Boil",
                                      &mEffectPos[mEffectIndex]);
            f32 scale = al::getRandom(0.75f, 1.0f);
            mEffectScale[mEffectIndex] = scale;
            al::setEffectParticleScale(mEffectGroup->getEffect(mEffectIndex), "Boil", scale);
            mEffectIndex = mEffectIndex >= 18 ? 0 : mEffectIndex + 1;
            mEmitEffectTimer = 0;
        }
    }

    for (s32 i = 0; i < 20; i++) {
        sead::Vector3f surface_pos;
        sead::Vector3f unused;
        al::calcFindFireSurface(&surface_pos, &unused, this, mEffectPos[i], sead::Vector3f::ey,
                                1000);
        surface_pos.y -= mEffectScale[i] * 150;
        mEffectPos[i].set(surface_pos);

        if (mEffectTimer[i] > 0) {
            --mEffectTimer[i];

            if (mEffectTimer[i] == 0) {
                sead::Vector2i coord = {0, 0};
                calcCoord(&coord, surface_pos, mTextureSize);
                mFluidSimulateWave->storePoint(coord, _0x180, _0x184 * mEffectScale[i], false);
            }
        }
    }

    mHeightMapTextureBlender->setSource(mSineWave->get_0x50(), 0);
    mHeightMapTextureBlender->setSource(mFluidSimulateWave->get_0x2d8(), 1);
    mEffectGroup->update();
}

void LavaWave::calcCoord(sead::Vector2i* out, const sead::Vector3f& pos, s32 texture_size) const {
    f32 size_x = (mRadius + mRadius) * al::getScale(this).x;
    f32 size_y = (mRadius + mRadius) * al::getScale(this).y;
    sead::Vector2f coord;
    al::calcOrthoProjectedTexCoord(&coord, *getBaseMtx(), al::getTrans(this), pos, size_x, size_y);
    out->set(coord.x * texture_size, coord.y * texture_size);
}

void LavaWave::startEndDemoBossMagma() {
    mWaveParams.x *= 0.2f;
    mSineWave->setWaveParams(mWaveParams.x, mWaveParams.y, mWaveParams.z, mWaveParams.w);
}

void LavaWave::setEndDemoParamBossMagma(const sead::Vector3f& pos, s32 param) {
    mBossMagmaEndDemoParam = param;
    sead::Vector3f unused = pos;
    calcPos(&unused, pos);
}

void LavaWave::endEndDemoBossMagma() {
    mBossMagmaEndDemoParam = -1;
    mWaveParams.x *= 5;
    mSineWave->setWaveParams(mWaveParams.x, mWaveParams.y, mWaveParams.z, mWaveParams.w);
}

void LavaWave::tryKillEmitterAndParticleAll() {
    for (s32 i = 0; i < mEffectGroup->getEffectNum(); i++)
        al::tryKillEmitterAndParticleAll(mEffectGroup->getEffect(i));
}

bool LavaWave::calcIsInArea(const sead::Vector3f& pos) const {
    sead::Vector3f rel_pos = -al::getTrans(this) + pos;
    if (rel_pos.x * rel_pos.x + rel_pos.z * rel_pos.z > mRadius * mRadius)
        return false;
    sead::Vector3f projected_pos;
    calcPos(&projected_pos, pos);
    return projected_pos.y > pos.y && projected_pos.y - 2000 < pos.y;
}

void LavaWave::calcPos(sead::Vector3f* out, const sead::Vector3f& pos) const {
    sead::Vector2i coord = {0, 0};
    calcCoord(&coord, pos, mTextureSize);
    if (coord.x >= mTextureSize || coord.y >= mTextureSize || coord.x < 0 || coord.y < 0) {
        out->set(pos);
        return;
    }
    f32 y = al::getF32FromLinearTextureF16(mSineWave->get_0x40(), coord.x, coord.y, 0, 1) +
            al::getTrans(this).y;
    out->set(pos.x, y, pos.z);
}

void LavaWave::calcPosFlat(sead::Vector3f* out, const sead::Vector3f& pos) const {
    out->set(pos.x, al::getTrans(this).y, pos.z);
}

void LavaWave::calcNormal(sead::Vector3f* out, const sead::Vector3f& pos) const {
    sead::Vector2i coord = {0, 0};
    calcCoord(&coord, pos, mTextureSize);
    al::calcNormalFromLinearTexture(out, mSineWave->get_0x48(), coord.x, coord.y);
}

constexpr f32 DefaultMaxDepth = 150;
constexpr f32 DefaultMaxHeight = 200;

bool LavaWave::tryAddRipple(const sead::Vector3f& pos, f32 _s0, f32 _s1) {
    return tryAddRippleWithRange(pos, _s0, _s1, DefaultMaxDepth, DefaultMaxHeight);
}

bool LavaWave::tryAddRippleWithRange(const sead::Vector3f& pos, f32 _s0, f32 _s1, f32 max_depth,
                                     f32 max_height) {
    sead::Vector3f rel_pos = -al::getTrans(this) + pos;
    if (rel_pos.x * rel_pos.x + rel_pos.z * rel_pos.z > mRadius * mRadius)
        return false;
    sead::Vector3f projected_pos;
    calcPos(&projected_pos, pos);
    f32 height = pos.y - projected_pos.y;
    if (height > max_height || height < -max_depth)
        return false;
    f32 _neg_s0 = _s0 > 0 ? -_s0 : _s0;
    sead::Vector2i coord = {0, 0};
    calcCoord(&coord, pos, mTextureSize);
    mFluidSimulateWave->storePoint(coord, _neg_s0 * 1400, _s1 * 1.6f, false);
    return true;
}

bool LavaWave::tryAddQuadRipple(const sead::Vector3f& p1, const sead::Vector3f& p2,
                                const sead::Vector3f& p3, const sead::Vector3f& p4, f32 _s0) {
    sead::Vector3f quad[] = {p1, p2, p3, p4};
    for (s32 i = 0; i < 4; i++) {
        const sead::Vector3f& point = quad[i];
        sead::Vector3f projected_pos;
        calcPos(&projected_pos, point);
        f32 height = point.y - projected_pos.y;
        if (height > DefaultMaxHeight || height < -DefaultMaxDepth)
            return false;
    }
    f32 _neg_s0 = _s0 > 0 ? -_s0 : _s0;
    for (const sead::Vector3f& point : quad) {
        sead::Vector2i coord = {0, 0};
        calcCoord(&coord, point, mTextureSize);
        mFluidSimulateWave->storePoint(coord, _neg_s0 * 1400, 1.0f, true);
    }
    return true;
}
