#include "Item/ShineInfo.h"

#include "Library/Base/StringUtil.h"
#include "Library/LiveActor/LiveActor.h"
#include "Library/Placement/PlacementFunction.h"
#include "Library/Placement/PlacementId.h"

#include "System/GameDataHolderAccessor.h"
#include "System/UniqObjInfo.h"
#include "Util/StageLayoutFunction.h"

ShineInfo::ShineInfo() {
    clear();
}

void ShineInfo::clear() {
    mStageName.clear();
    mObjId.clear();
    mLabel.clear();
    mType = Type_Shine;
    mQuestInfo = nullptr;
    mIsShop = false;
    mGetTime = 0;
}

void ShineInfo::init(const char* stage_name, const char* obj_id, const char* label,
                     const QuestInfo* quest_info, s32 type) {
    clear();
    mStageName.format("%s", stage_name);
    mObjId.format("%s", obj_id);
    if (label)
        mLabel.format("%s", label);
    mType = static_cast<Type>(type);
    mQuestInfo = quest_info;
}

void ShineInfo::initForHintPhoto(const al::LiveActor* actor, const al::IUseMessageSystem*,
                                 const al::ActorInitInfo& info) {
    clear();
    al::PlacementId placement_id;
    al::tryGetPlacementId(&placement_id, info);
    al::StringTmp<128> obj_id;
    al::StringTmp<128> label;
    obj_id.format("%s", placement_id.getId());
    label.format("ScenarioName_%s", obj_id.cstr());
    init(rs::getPlacementStageName(actor, info), obj_id.cstr(), label.cstr(), nullptr, Type_Shine);
}

bool ShineInfo::isEmpty() const {
    return mStageName.isEmpty() && mObjId.isEmpty();
}

bool ShineInfo::isEqual(const ShineInfo* other) const {
    if (al::isEqualString(other->mStageName.cstr(), mStageName.cstr()) &&
        al::isEqualString(other->mObjId.cstr(), mObjId.cstr()))
        return true;
    return false;
}

bool ShineInfo::isEqual(const ShineData* other) const {
    if (al::isEqualString(other->stageName, mStageName.cstr()) &&
        al::isEqualString(other->objId, mObjId.cstr()))
        return true;
    return false;
}

void ShineInfo::writeShineData(ShineData* dest) const {
    al::copyString(dest->stageName, mStageName.cstr(), 128);
    al::copyString(dest->objId, mObjId.cstr(), 128);
}

void ShineInfo::readShineData(const ShineData* src) {
    mStageName.format("%s", src->stageName);
    mObjId.format("%s", src->objId);
}

void ShineInfo::readShineData(const UniqObjInfo* src) {
    mStageName.format("%s", src->getStageName());
    mObjId.format("%s", src->getObjId());
}

void ShineInfo::copyShineInfo(const ShineInfo* src) {
    mStageName.format("%s", src->mStageName.cstr());
    mObjId.format("%s", src->mObjId.cstr());
    mLabel.format("%s", src->mLabel.cstr());
    mType = src->mType;
    mQuestInfo = src->mQuestInfo;
    mGetTime = src->mGetTime;
}
