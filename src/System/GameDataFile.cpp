#include "System/GameDataFile.h"

#include "Library/Message/MessageHolder.h"
#include "Library/Placement/PlacementFunction.h"
#include "Library/Placement/PlacementId.h"
#include "Library/Placement/PlacementInfo.h"
#include "Library/Resource/ResourceFunction.h"
#include "Library/Yaml/ByamlUtil.h"
#include "Library/Yaml/Writer/ByamlWriter.h"

#include "Amiibo/SearchAmiiboDataTable.h"
#include "Item/ShineInfo.h"
#include "Layout/ShopLayoutInfo.h"
#include "MapObj/ChangeStageInfo.h"
#include "Npc/SessionMusicianType.h"
#include "Scene/QuestInfo.h"
#include "Scene/QuestInfoHolder.h"
#include "Sequence/GameSequenceInfo.h"
#include "System/AchievementSaveData.h"
#include "System/BossSaveData.h"
#include "System/CollectBgm.h"
#include "System/CollectBgmBailoutInfo.h"
#include "System/GameDataFunction.h"
#include "System/GameDataUtil.h"
#include "System/GameProgressData.h"
#include "System/HintPhotoData.h"
#include "System/MoonRockData.h"
#include "System/NetworkUploadFlag.h"
#include "System/PlayerHitPointData.h"
#include "System/RaceRecord.h"
#include "System/SequenceDemoSkipData.h"
#include "System/ShopTalkData.h"
#include "System/SphinxQuizData.h"
#include "System/TimeBalloonSaveData.h"
#include "System/VisitStageData.h"
#include "System/WorldList.h"
#include "System/WorldWarpTalkData.h"
#include "Util/AchievementUtil.h"
#include "Util/StageLayoutFunction.h"

void GameDataFile::HintInfo::clear() {
    stageName.clear();
    objId.clear();
    scenarioName = nullptr;
    trans.set(sead::Vector3f::zero);
    originalTrans.set(sead::Vector3f::zero);
    mainScenarioNo = -1;
    worldId = -1;
    isMoonRock = false;
    isGet = false;
    getTime = 0;
    uniqueId = -1;
    hintStatus = HintStatus_Lock;
    isAchievement = false;
    isGrand = false;
    status = AchievementStatus_None;
    isShop = false;
    hintIdx = 0;
    optionalId.clear();
    progressBitFlag = 0;
    isDisabled = false;
    isEnableHintInCeremony = false;
}

bool GameDataFile::HintInfo::isDisableByWorldWarpHole(bool is_game_clear) const {
    // TODO cleanup
    if (!is_game_clear && al::isEqualString(optionalId.cstr(), "WorldWarpHoleShine"))
        return true;
    return false;
}

bool GameDataFile::HintInfo::isEnableUnlock(s32 world_id, bool is_moon_rock, s32 scenario_no,
                                            bool is_game_clear) const {
    if (isDisableByWorldWarpHole(is_game_clear))
        return false;
    if (worldId != world_id)
        return false;
    if (isDisabled)
        return false;
    if (!(is_moon_rock || progressBitFlag.countOnBit() == 0 ||
          progressBitFlag.isOnBit(scenario_no - 1)))
        return false;
    if (isGet)
        return false;
    if (hintStatus != HintStatus_Lock)
        return false;
    if (is_moon_rock ? isMoonRock : !isMoonRock)
        return true;
    return false;
}

bool GameDataFile::HintInfo::isHintStatusUnlock(s32 world_id, s32 scenario_no,
                                                bool is_moon_rock) const {
    if (worldId != world_id)
        return false;
    if (isGet)
        return false;
    if (hintStatus == HintStatus_Lock)
        return false;
    if (is_moon_rock ? isMoonRock : !isMoonRock) {
        if (GameDataFunction::isCityWorldCeremonyAll(world_id, scenario_no))
            return isEnableHintInCeremony;
        return true;
    }
    return false;
}

bool GameDataFile::HintInfo::isHintStatusUnlockByNpc() const {
    return hintStatus == HintStatus_UnlockByNpc && !isMoonRock;
}

bool GameDataFile::HintInfo::isHintStatusUnlockByAmiibo() const {
    return hintStatus == HintStatus_UnlockByAmiibo;
}

bool GameDataFile::HintInfo::isEnableNameUnlockByScenario(s32 world_id, s32 scenario_no,
                                                          bool is_game_clear) const {
    if (isDisableByWorldWarpHole(is_game_clear))
        return false;
    if (worldId != world_id)
        return false;
    if (isDisabled)
        return false;
    return progressBitFlag.countOnBit() == 0 || progressBitFlag.isOnBit(scenario_no - 1);
}

void GameDataFile::CoinCollectInfo::clear() {
    worldId = -1;
    stageName.clear();
    objId.clear();
    uniqueId = -1;
    isGet = false;
}

static s32 compareStringOrNull(const char* a, const char* b) {
    return a ? b ? std::strcmp(a, b) : 1 : b ? -1 : 0;
}

static s32 compareCollectBgmInfo(const GameDataFile::CollectBgmInfo* a,
                                 const GameDataFile::CollectBgmInfo* b) {
    s32 result = std::strcmp(a->name, b->name);
    if (result != 0)
        return result;
    return compareStringOrNull(a->situationName, b->situationName);
}

GameDataFile::GameDataFile(GameDataHolder* game_data_holder) : mGameDataHolder(game_data_holder) {
    mGotShine.alloc();
    mGotGrandShine.alloc();
    mLatestGetShineInfo = new ShineInfo();
    for (s32 i = 0; i < 1024; i++)
        mGotShine[i] = new ShineInfo();
    for (s32 i = 0; i < 1024; i++)
        mGotGrandShine[i] = new ShineInfo();

    mStartedObj.alloc();
    mSaveObjS32.alloc();
    mGrowFlower.alloc();
    mPlayerHitPointData = new PlayerHitPointData();
    mGotCheckpoint.alloc();
    mHintList.alloc();

    s32 world_num = mGameDataHolder->getWorldList()->getWorldNum();
    mHintTable.allocBuffer(world_num, nullptr);
    mHintTableByIdx.allocBuffer(world_num, nullptr);
    mCoinCollectTable.allocBuffer(world_num, nullptr);

    mCoinCollectList.allocBuffer(world_num * 100, nullptr);
    for (s32 i = 0; i < mCoinCollectList.capacity(); i++)
        mCoinCollectList.pushBack(new CoinCollectInfo());

    for (s32 i = 0; i < world_num; i++) {
        mHintTable.pushBack(new WorldHintList());
        mHintTable[i]->index = i;
        mHintTable[i]->list.allocBuffer(100, nullptr);

        mHintTableByIdx.pushBack(new WorldHintList());
        mHintTableByIdx[i]->index = i;
        mHintTableByIdx[i]->list.allocBuffer(100, nullptr);

        mCoinCollectTable.pushBack(new WorldCoinCollectList());
        mCoinCollectTable[i]->index = i;
        mCoinCollectTable[i]->list.allocBuffer(100, nullptr);
    }

    mUnlockedWorldId.alloc();
    mCoinCollectGotNum.alloc();
    mUseCoinCollectNum.alloc();
    mScenarioNo.alloc();

    mWorldWarpTalkData = new WorldWarpTalkData(sNumWorlds);
    mVisitStageData = new VisitStageData();
    mGameProgressData = new GameProgressData(mGameDataHolder->getWorldList());
    mMoonRockData = new MoonRockData(mGameDataHolder->getWorldList()->getWorldNum());
    mBossSaveData = new BossSaveData();
    mAchievementSaveData = new AchievementSaveData(mGameDataHolder->getAchievementInfoReader());
    mSearchAmiiboDataTable = new SearchAmiiboDataTable();
    mNetworkUploadFlag = new NetworkUploadFlag();
    mSequenceDemoSkipData = new SequenceDemoSkipData(mGameDataHolder->getWorldList());
    mHintPhotoData = new HintPhotoData(this);

    mMainScenarioNo.alloc();
    mIsWorldWarpHoleThrough.alloc();
    mSaveTimeForDisp = 0;
    mSaveTime = 0;
    mShineNum.alloc();
    mShopShineNum.alloc();
    mPayShineNum.alloc();

    mCheckpointTable = new CheckpointInfo*[sNumWorlds];
    for (s32 i = 0; i < sNumWorlds; i++)
        mCheckpointTable[i] = new CheckpointInfo[16]();

    mItemCap.alloc();
    mItemCloth.alloc();
    mItemSticker.alloc();
    mItemGift.alloc();
    mHackDictionary.alloc();
    mTotalShineNum.alloc();
    mTotalMoonRockShineNum.alloc();

    mTimeBalloonSaveData = new TimeBalloonSaveData();
    mIsExistSessionMember.alloc();
    mSphinxQuizData = new SphinxQuizData(GameDataFunction::getWorldNum(mGameDataHolder));

    mRaceRecord.allocBuffer(23, nullptr);

    RaceRecord* record_yukimaru_1 = new RaceRecord();
    record_yukimaru_1->setName("Yukimaru_1");
    mRaceRecord.pushBack(record_yukimaru_1);

    RaceRecord* record_yukimaru_2 = new RaceRecord();
    record_yukimaru_2->setName("Yukimaru_2");
    mRaceRecord.pushBack(record_yukimaru_2);

    RaceRecord* record_radicon = new RaceRecord();
    record_radicon->setName("Radicon");
    mRaceRecord.pushBack(record_radicon);

    for (s32 i = 0; i < 20; i++) {
        RaceRecord* record = new RaceRecord();
        record->setName(al::StringTmp<32>("RaceManRace_%d", i).cstr());
        mRaceRecord.pushBack(record);
    }

    mWorldMapIndex.alloc();
    mWorldWarpIndex.alloc();
    mShopNpcInfo = new ShopNpcInfo[4];
    mShopTalkData = new ShopTalkData(20, mGameDataHolder->getShopsMoonNum(),
                                     mGameDataHolder->getShopTalkDataSize(),
                                     mGameDataHolder->getShopItemInfoList());
    mMiniGameInfo.alloc();
    mChangeStageInfo = new ChangeStageInfo(mGameDataHolder, "", "");
    mMissRestartInfo = new ChangeStageInfo(mGameDataHolder, "", "");
    mFlagTalkMessage.alloc();
    mIsTalkLocalLanguage.alloc();

    s32 size = cCollectBgmListSize;
    mCollectBgmList.allocBuffer(size, nullptr);
    for (s64 i = 0; i < size; i++)
        mCollectBgmList.pushBack(
            new CollectBgmInfo(cCollectBgmList[i].name, cCollectBgmList[i].situationName));
    mCollectBgmList.sort(compareCollectBgmInfo);

    initializeData();
}

static void clampCount(s32* value, s32 margin = 0) {
    *value = sead::Mathi::clamp(*value, 0, sead::Mathi::maxNumber() - 1 - margin);
}

static bool isEqual(const GameDataFile::HintInfo* hint_info, const ShineInfo* shine_info) {
    return al::isEqualString(hint_info->stageName.cstr(), shine_info->getStageName()) &&
           al::isEqualString(hint_info->objId.cstr(), shine_info->getObjId());
}

static s32 tryFindShineIndexImpl(const GameDataFile::HintInfo* hint_list,
                                 const ShineInfo* shine_info) {
    for (s32 i = 0; i < 1024; i++)
        if (isEqual(&hint_list[i], shine_info))
            return i;
    return -1;
}

static GameDataFile::CheckpointInfo* tryFindCheckpointInfoImpl(GameDataFile::CheckpointInfo** table,
                                                               const char* stage_name,
                                                               const char* obj_id) {
    for (s32 i = 0; i < sNumWorlds; i++)
        for (s32 j = 0; j < 16; j++)
            if (al::isEqualString(stage_name, table[i][j].objInfo.stageName) &&
                al::isEqualString(obj_id, table[i][j].objInfo.getObjId()))
                return &table[i][j];
    return nullptr;
}

static void readUniqObjInfo(UniqObjInfo* out, const al::ByamlIter& iter, s32 index) {
    al::ByamlIter subiter;
    if (iter.tryGetIterByIndex(&subiter, index)) {
        out->setStageName(al::getByamlKeyString(subiter, "StageName"));
        out->setObjId(al::getByamlKeyString(subiter, "ObjId"));
    }
}

static void readSaveObjS32(GameDataFile::SaveObjS32* out, const al::ByamlIter& iter, s32 index) {
    al::ByamlIter subiter;
    if (iter.tryGetIterByIndex(&subiter, index)) {
        out->objInfo.setStageName(al::getByamlKeyString(subiter, "StageName"));
        out->objInfo.setObjId(al::getByamlKeyString(subiter, "ObjId"));
        out->value = al::getByamlKeyInt(subiter, "Value");
    }
}

static void readShine(UniqObjInfo* out_obj_info, u64* out_time, s32* out_status,
                      const al::ByamlIter& iter, s32 index) {
    al::ByamlIter subiter;
    if (iter.tryGetIterByIndex(&subiter, index)) {
        out_obj_info->setStageName(al::getByamlKeyString(subiter, "StageName"));
        out_obj_info->setObjId(al::getByamlKeyString(subiter, "ObjId"));
        iter.tryGetUInt64ByKey(out_time, "Time");
        iter.tryGetIntByKey(out_status, "Status");
    }
}

static void readItemList(const al::ByamlIter& iter, const char* name,
                         sead::FixedSafeString<64>* item_list) {
    al::ByamlIter item_list_iter;
    iter.tryGetIterByKey(&item_list_iter, name);
    for (s32 i = 0; i < 64; i++) {
        const char* str = nullptr;
        item_list_iter.tryGetStringByIndex(&str, i);
        if (!str)
            break;
        item_list[i].format(str);
    }
}

bool GameDataFile::tryReadByamlData(const u8* data) {
    if (!alByamlLocalUtil::verifiByaml(data))
        return false;

    // sp+0x280
    al::ByamlIter iter{data};
    if (!iter.isValid())
        return false;
    mStartShineIndex = al::getByamlKeyInt(iter, "StartShineIndex");
    if (const char* str = al::tryGetByamlKeyStringOrNULL(iter, "PlayerStartIdForSave"))
        mPlayerStartIdForSave.format("%s", str);
    else
        mPlayerStartIdForSave.clear();
    iter.tryGetUInt64ByKey(&mSaveTimeForDisp, "LastUpdateTime");
    iter.tryGetInt64ByKey(&mSaveDataIdForPrepo, "SaveDataIdForPrepo");

    mTotalPayShineNumForDisp = 0;
    // sp+0x270
    al::ByamlIter world_info_iter;
    iter.tryGetIterByKey(&world_info_iter, "WorldInfo");
    for (s32 i = 0; i < sNumWorlds; i++) {
        // sp+0x260
        al::ByamlIter subiter;
        world_info_iter.tryGetIterByIndex(&subiter, i);
        al::tryGetByamlS32(&mUnlockedWorldId[i], subiter, "UnlockedWorldId");
        if (i != 0 && mUnlockedWorldId[i] >= 0)
            mUnlockedWorldNum++;
        al::tryGetByamlS32(&mMainScenarioNo[i], subiter, "MainShineNo");
        al::tryGetByamlS32(&mPayShineNum[i], subiter, "PayShineNum");
        mTotalPayShineNumForDisp =
            sead::Mathi::clampMax(mTotalPayShineNumForDisp + mPayShineNum[i], 999);
        al::tryGetByamlS32(&mScenarioNo[i], subiter, "ScenarioNo");
        mScenarioNo[i] = sead::Mathi::max(mScenarioNo[i], 1);
        al::tryGetByamlS32(&mUseCoinCollectNum[i], subiter, "UseCoinCollectNum");
        // sp+0x250
        al::ByamlIter coin_collect_iter;
        subiter.tryGetIterByKey(&coin_collect_iter, "CoinCollect");
        for (s32 j = 0; j < coin_collect_iter.getSize(); j++) {
            // sp+0x120
            UniqObjInfo obj_info;
            readUniqObjInfo(&obj_info, coin_collect_iter, j);
            if (CoinCollectInfo* info =
                    tryFindCoinCollectInfo(obj_info.getStageName(), obj_info.getObjId()))
                info->isGet = true;
            mCoinCollectGotNum[i]++;
        }
        mShopShineNum[i] = al::tryGetByamlKeyIntOrZero(subiter, "ShopShine");
        clampCount(&mShopShineNum[i]);
        clampCount(&mShineNum[i], mShopShineNum[i]);
        mShineNum[i] += mShopShineNum[i];
        al::tryGetByamlBool(&mIsTalkLocalLanguage[i], subiter, "TalkLocalLanguage");
        al::tryGetByamlBool(&mIsWorldWarpHoleThrough[i], subiter, "WorldWarpHoleThrough");
    }

    mCurrentWorldId = sead::Mathi::max(al::tryGetByamlKeyIntOrZero(iter, "WorldId"), 0);
    mCurrentWorldIdForWrite = mCurrentWorldId;
    mCurrentWorldIdForDisp = mCurrentWorldId;
    mNextStageName.format("%s", mGameDataHolder->getWorldList()->getMainStageName(mCurrentWorldId));
    mCurrentStageName.format("%s",
                             mGameDataHolder->getWorldList()->getMainStageName(mCurrentWorldId));
    al::tryGetByamlS64(&mPlayTimeTotal, iter, "PlayTimeTotal");
    if (mPlayTimeTotal < 0)
        mPlayTimeTotal = 0;

    // sp+0x260
    al::ByamlIter shine_data_iter;
    iter.tryGetIterByKey(&shine_data_iter, "ShineData");
    for (s32 i = 0; i < shine_data_iter.getSize(); i++) {
        // sp+0x120
        al::ByamlIter subiter;
        shine_data_iter.tryGetIterByIndex(&subiter, i);
        // sp+0x290
        s32 unique_id = 0;
        // sp+0x250
        s32 status = 0;
        al::tryGetByamlS32(&unique_id, subiter, "UniqueId");
        al::tryGetByamlS32(&status, subiter, "Status");
        s32 index = tryFindShineIndexByUniqueId(unique_id);
        if (index < 0)
            continue;
        mHintList[index].status = static_cast<AchievementStatus>(status);
        al::tryGetByamlS32(&mHintList[index].hintStatus, subiter, "HintStatus");
        al::tryGetByamlU64(&mHintList[index].getTime, subiter, "GetTime");
        if (al::tryGetByamlBool(&mHintList[index].isGet, subiter, "IsGet") &&
            mHintList[index].isGet) {
            if (mHintList[index].isGrand)
                mShineNum[mHintList[index].worldId] += 3;
            else if (!mHintList[index].isShop)
                mShineNum[mHintList[index].worldId]++;
        }
    }

    // sp+0x250
    al::ByamlIter shine_iter;
    iter.tryGetIterByKey(&shine_iter, "Shine");
    for (s32 i = 0; i < shine_iter.getSize(); i++) {
        // sp+0x120
        UniqObjInfo obj_info;
        // sp+0x290
        u64 time = 0;
        // sp+0x110
        s32 status = 0;
        readShine(&obj_info, &time, &status, shine_iter, i);
        mGotShine[i]->readShineData(&obj_info);
        if (mGotShine[i]->isEmpty())
            continue;
        s32 index = tryFindShineIndexImpl(mHintList.begin(), mGotShine[i]);
        if (index == -1)
            continue;
        s32 world_id = mHintList[index].worldId;
        mHintList[index].isGet = true;
        if (time != 0)
            mHintList[index].getTime = time;
        mHintList[index].status = AchievementStatus_Got;
        if (world_id >= 0) {
            mShineNum[world_id]++;
            if (world_id == mCurrentWorldId)
                for (s32 j = 0; j < 1024; j++)
                    isEqual(&mHintList[j], mGotShine[i]);
        }
    }

    // sp+0x110
    al::ByamlIter get_coin_collect_data_iter;
    iter.tryGetIterByKey(&get_coin_collect_data_iter, "GetCoinCollectData");
    for (s32 i = 0; i < get_coin_collect_data_iter.getSize(); i++) {
        // sp+0x120
        al::ByamlIter subiter;
        get_coin_collect_data_iter.tryGetIterByIndex(&subiter, i);
        // sp+0x290
        s32 unique_id = 0;
        al::tryGetByamlS32(&unique_id, subiter, "UniqueId");
        s32 index = tryFindCoinCollectIndexByUniqueId(unique_id);
        if (index < 0)
            continue;
        if (al::tryGetByamlBool(&mCoinCollectList[index]->isGet, subiter, "IsGet") &&
            mCoinCollectList[index]->isGet)
            mCoinCollectGotNum[mCoinCollectList[index]->worldId]++;
    }

    mPlayerStartId.format("%s", mPlayerStartIdForSave.cstr());

    // sp+0x100
    al::ByamlIter grand_shine_iter;
    iter.tryGetIterByKey(&grand_shine_iter, "GrandShine");
    for (s32 i = 0; i < grand_shine_iter.getSize(); i++) {
        // sp+0x120
        UniqObjInfo obj_info;
        // sp+0x290
        u64 time = 0;
        // sp+0x110
        s32 status = 0;
        readShine(&obj_info, &time, &status, grand_shine_iter, i);
        mGotGrandShine[i]->readShineData(&obj_info);
        mGotGrandShine[i]->setType(ShineInfo::Type_GrandShine);
        if (mGotGrandShine[i]->isEmpty())
            continue;
        s32 index = tryFindShineIndexImpl(mHintList.begin(), mGotGrandShine[i]);
        if (index == -1)
            continue;
        s32 world_id = mHintList[index].worldId;
        mHintList[index].isGet = true;
        if (time != 0)
            mHintList[index].getTime = time;
        mHintList[index].status = AchievementStatus_Got;
        if (world_id >= 0)
            mShineNum[world_id] += 3;
    }

    // sp+0xf0
    al::ByamlIter started_obj_iter;
    iter.tryGetIterByKey(&started_obj_iter, "StartedObj");
    for (s32 i = 0; i < started_obj_iter.getSize(); i++)
        readUniqObjInfo(&mStartedObj[i], started_obj_iter, i);

    // sp+0xe0
    al::ByamlIter save_obj_s32_iter;
    iter.tryGetIterByKey(&save_obj_s32_iter, "SaveObjS32");
    for (s32 i = 0; i < save_obj_s32_iter.getSize(); i++)
        readSaveObjS32(&mSaveObjS32[i], save_obj_s32_iter, i);

    // sp+0xd0
    al::ByamlIter checkpoint_iter;
    iter.tryGetIterByKey(&checkpoint_iter, "Checkpoint");
    mGotCheckpointNum = 0;
    for (s32 i = 0; i < 320; i++) {
        mGotCheckpoint[i].clear();
        // sp+0x120
        UniqObjInfo obj_info;
        readUniqObjInfo(&obj_info, checkpoint_iter, i);
        CheckpointInfo* info = tryFindCheckpointInfoImpl(mCheckpointTable, obj_info.getStageName(),
                                                         obj_info.getObjId());
        if (!info || info->objInfo.stageName.isEmpty())
            continue;
        mGotCheckpoint[i].setStageName(info->objInfo.getStageName());
        mGotCheckpoint[i].objId.format("%s", info->objInfo.getObjId());
        mGotCheckpointNum++;
        info->isGet = true;
    }

    al::tryGetByamlS32(&mCoinNum, iter, "CoinNum");
    al::tryGetByamlS32(&mTotalCoinNum, iter, "TotalCoinNum");
    clampCount(&mTotalCoinNum);
    al::tryGetByamlS32(&mPlayerJumpCount, iter, "PlayerJumpCount");
    clampCount(&mPlayerJumpCount);
    al::tryGetByamlS32(&mPlayerThrowCapCount, iter, "PlayerThrowCapCount");
    clampCount(&mPlayerThrowCapCount);

    readItemList(iter, "ItemCap", mItemCap.begin());
    readItemList(iter, "ItemCloth", mItemCloth.begin());
    readItemList(iter, "ItemSticker", mItemSticker.begin());
    readItemList(iter, "ItemGift", mItemGift.begin());
    buyDefaultItem();

    if (const char* str = al::tryGetByamlKeyStringOrNULL(iter, "CurrentClothName"))
        mCurrentCostumeName.format("%s", str);
    if (const char* str = al::tryGetByamlKeyStringOrNULL(iter, "CurrentCapName"))
        mCurrentCapName.format("%s", str);
    mIsCostumeRandom = al::tryGetByamlKeyBoolOrFalse(iter, "IsCostumeRandom");
    mIsCapRandom = al::tryGetByamlKeyBoolOrFalse(iter, "IsCapRandom");

    // sp+0x290
    al::ByamlIter hack_dictionary_iter;
    iter.tryGetIterByKey(&hack_dictionary_iter, "HackDictionary");
    for (s32 i = 0; i < 160; i++) {
        // sp+0x120
        const char* str = nullptr;
        if (hack_dictionary_iter.tryGetStringByIndex(&str, i))
            mHackDictionary[i].format("%s", str);
    }

    mIsRideMotorcycle = al::tryGetByamlKeyBoolOrFalse(iter, "IsRideMotorcycle");
    mIsRideSphinx = al::tryGetByamlKeyBoolOrFalse(iter, "IsRideSphinx");

    // sp+0xc0
    al::ByamlIter session_iter;
    iter.tryGetIterByKey(&session_iter, "Session");
    for (s32 i = 0; i < session_iter.getSize(); i++)
        // sp+0x120
        if (s32 value = 0; session_iter.tryGetIntByIndex(&value, i))
            mIsExistSessionMember[value] = true;

    // sp+0xbc
    s32 session_event_progress = 0;
    al::tryGetByamlS32(&session_event_progress, iter, "SessionEventProgress");
    mSessionEventProgress = session_event_progress =
        sead::Mathi::min(session_event_progress, SessionEventProgress::getLastIndex());
    mIsPayCoinToSphinx = al::tryGetByamlKeyBoolOrFalse(iter, "IsPayCoinToSphinx");
    mIsHintNpcFirstTalk = al::tryGetByamlKeyBoolOrFalse(iter, "IsHintNpcFirstTalk");
    mIsKinopioBrigadeNpcFirstTalk =
        al::tryGetByamlKeyBoolOrFalse(iter, "IsKinopioBrigadeNpcFirstTalk");
    mCapStatusForJango =
        static_cast<CapStatus>(al::tryGetByamlKeyIntOrZero(iter, "CapStatusForJango"));

    mTimeBalloonSaveData->read(iter);

    // sp+0xb0
    const char* world_traveling_status = nullptr;
    if (al::tryGetByamlString(&world_traveling_status, iter, "WorldTravelingStatus"))
        mWorldTravelingStatus.format("%s", world_traveling_status);
    al::tryGetByamlBool(&mIsStartWorldTravelingPeach, iter, "IsStartWorldTravelingPeach");
    al::tryGetByamlBool(&mIsPlayAlreadyWorldWarp, iter, "IsPlayAlreadyWorldWarp");
    al::tryGetByamlBool(&mIsTalkFirstAmiiboNpc, iter, "IsTalkFirstAmiiboNpc");
    al::tryGetByamlBool(&mIsUnlockAchievement, iter, "IsUnlockAchievement");
    if (mIsUnlockAchievement)
        unlockAchievementShineName();

    mWorldWarpTalkData->read(iter);
    mVisitStageData->read(iter);
    mGameProgressData->read(iter);
    mMoonRockData->read(iter);
    mBossSaveData->read(iter);
    mShopTalkData->read(iter);
    mAchievementSaveData->read(iter);
    clampCount(&mShineNum[GameDataFunction::getWorldIndexPeach()],
               mAchievementSaveData->calcGetAchievementNum());
    mShineNum[GameDataFunction::getWorldIndexPeach()] +=
        mAchievementSaveData->calcGetAchievementNum();
    mSearchAmiiboDataTable->read(iter);
    mNetworkUploadFlag->read(iter);
    mSequenceDemoSkipData->read(iter);
    mHintPhotoData->read(iter);

    mIsPlayDemoOpening = al::getByamlKeyBool(iter, "IsPlayDemoOpening");
    mIsMeetCap = al::getByamlKeyBool(iter, "IsEnableCap");
    mNextWorldId = al::tryGetByamlKeyIntOrZero(iter, "NextWorldId");
    mPrevWorldId = al::tryGetByamlKeyIntOrZero(iter, "PrevWorldId");
    mJangoCount = al::tryGetByamlKeyIntOrZero(iter, "JangoCount");
    mIsPayShineAllInAllWorld = al::tryGetByamlKeyBoolOrFalse(iter, "IsPayShineAllInAllWorld");

    // sp+0xa0
    al::ByamlIter race_record_iter;
    iter.tryGetIterByKey(&race_record_iter, "RaceRecord");
    for (s32 i = 0; i < mRaceRecord.size(); i++)
        mRaceRecord[i]->read(race_record_iter);

    iter.tryGetIntByKey(&mRaceLoseCountLv1, "RaceLoseCountLv1");
    iter.tryGetIntByKey(&mRaceLoseCountLv2, "RaceLoseCountLv2");

    // sp+0x90
    al::ByamlIter sphinx_quiz_data_iter;
    iter.tryGetIterByKey(&sphinx_quiz_data_iter, "SphinxQuizData");
    mSphinxQuizData->read(sphinx_quiz_data_iter);

    iter.tryGetIntByKey(&mJumpingRopeBestCount, "JumpingRopeBestCount");
    iter.tryGetBoolByKey(&mIsExistRecordJumpingRope, "IsExistRecordJumpingRope");
    iter.tryGetIntByKey(&mVolleyballBestCount, "VolleyballBestCount");
    iter.tryGetBoolByKey(&mIsExistRecordVolleyball, "IsExistRecordVolleyball");
    iter.tryGetBoolByKey(&mIsPlayDemoPlayerDownForBattleKoopaAfter,
                         "IsPlayDemoPlayerDownForBattleKoopaAfter");
    iter.tryGetBoolByKey(&mIsShowExplainCheckpointFlag, "ShowExplainCheckpointFlag");

    // sp+0x80
    al::ByamlIter flag_talk_message_iter;
    iter.tryGetIterByKey(&flag_talk_message_iter, "FlagTalkMessage");
    for (s32 i = 0; i < 256; i++) {
        mFlagTalkMessage[i] = false;
        flag_talk_message_iter.tryGetBoolByIndex(&mFlagTalkMessage[i], i);
    }

    iter.tryGetBoolByKey(&mIsKidsMode, "IsKidsMode");
    mPlayerHitPointData->setKidsModeFlag(isKidsMode());
    mPlayerHitPointData->read(iter);

    // sp+0x70
    al::ByamlIter grow_pot_iter;
    iter.tryGetIterByKey(&grow_pot_iter, "GrowPot");
    for (s32 i = 0; i < 16; i++) {
        // sp+0x120
        al::ByamlIter subiter;
        grow_pot_iter.tryGetIterByIndex(&subiter, i);
        // sp+0x60
        const char* str = nullptr;
        if (subiter.tryGetStringByKey(&str, "PotObj"))
            mGrowFlower[i].potObj.format("%s", str);
        if (subiter.tryGetStringByKey(&str, "SeedObj"))
            mGrowFlower[i].seedObj.format("%s", str);
        subiter.tryGetUInt64ByKey(&mGrowFlower[i].time, "Time");
        subiter.tryGetUIntByKey(&mGrowFlower[i].growLevel, "AddGrow");
    }

    updateWorldMapIndex();
    updateWorldWarpIndex();
    if (al::isEqualString(
            "City", GameDataFunction::getWorldDevelopName(mGameDataHolder, mCurrentWorldId)) &&
        mSessionEventProgress != SessionEventProgress::Entry) {
        mPlayerStartIdForSave.clear();
        mPlayerStartId.clear();
    }
    if (al::isEqualString(
            "Snow", GameDataFunction::getWorldDevelopName(mGameDataHolder, mCurrentWorldId))) {
        mPlayerStartIdForSave.clear();
        mPlayerStartId.clear();
    }
    for (s32 i = 0; i < mGameDataHolder->getWorldList()->getWorldNum(); i++)
        mShineNum[i] = sead::Mathi::max(mPayShineNum[i], mShineNum[i]);

    // sp+0x60
    al::ByamlIter collect_bgm_list_iter;
    iter.tryGetIterByKey(&collect_bgm_list_iter, "CollectBgmList");
    for (s32 i = 0; i < collect_bgm_list_iter.getSize(); i++) {
        // sp+0x50
        al::ByamlIter subiter;
        collect_bgm_list_iter.tryGetIterByIndex(&subiter, i);
        CollectBgmInfo info;
        subiter.tryGetBoolByKey(&info.isCollected, "IsCollected");
        subiter.tryGetStringByKey(&info.name, "BgmResourceName");
        subiter.tryGetStringByKey(&info.situationName, "BgmSituationName");
        if (!info.name)
            continue;
        s32 index = mCollectBgmList.binarySearch(&info, compareCollectBgmInfo);
        if (index >= 0)
            mCollectBgmList[index]->isCollected = info.isCollected;
    }

    iter.tryGetBoolByKey(&mIsTalkKakku, "IsTalkKakku");
    iter.tryGetBoolByKey(&mIsTalkWorldTravelingPeach, "IsTalkWorldTravelingPeach");
    iter.tryGetBoolByKey(&mIsTalkCollectBgmNpc, "IsTalkCollectBgmNpc");
    iter.tryGetIntByKey(&mTokimekiMayorNpcFavorabilityRating, "TokimekiMayorNpcFavorabilityRating");
    iter.tryGetBoolByKey(&mIsFirstNetwork, "FirstNetwork");

    if (mCurrentWorldId == GameDataFunction::getWorldIndexSea() &&
        al::isEqualString(mCurrentStageName.cstr(),
                          mGameDataHolder->getWorldList()->getMainStageName(mCurrentWorldId)) &&
        getScenarioNo(getCurrentWorldIdNoDevelop()) == 1 &&
        getMainScenarioNo(mCurrentWorldId) == 1) {
        mPlayerStartId.format("GiantWanderBoss");
        mCheckpointName.clear();
        _908.clear();
    }

    if (isEmpty())
        initializeData();

    return true;
}

GameDataFile::CoinCollectInfo* GameDataFile::tryFindCoinCollectInfo(const char* stage_name,
                                                                    const char* obj_id) const {
    for (s32 i = 0; i < mCoinCollectList.capacity(); i++) {
        CoinCollectInfo* info = mCoinCollectList[i];
        if (info->stageName.isEqual(stage_name) && info->objId.isEqual(obj_id))
            return info;
    }
    return nullptr;
}

s32 GameDataFile::tryFindShineIndexByUniqueId(s32 unique_id) const {
    for (s32 i = 0; i < 1024; i++)
        if (mHintList[i].uniqueId == unique_id)
            return i;
    return -1;
}

s32 GameDataFile::tryFindCoinCollectIndexByUniqueId(s32 unique_id) const {
    for (s32 i = 0; i < mCoinCollectList.capacity(); i++)
        if (mCoinCollectList[i]->uniqueId == unique_id)
            return i;
    return -1;
}

void GameDataFile::unlockAchievementShineName() {
    for (s32 i = 0; i < mHintList.size(); i++)
        if (mHintList[i].status == AchievementStatus_None && mHintList[i].isAchievement)
            mHintList[i].status = AchievementStatus_Unlocked;
    mIsUnlockAchievement = true;
}

bool GameDataFile::isKidsMode() const {
    return mIsKidsMode;
}

s32 GameDataFile::getScenarioNo(s32 world_id) const {
    return mScenarioNo[world_id];
}

s32 GameDataFile::getMainScenarioNo(s32 world_id) const {
    return mMainScenarioNo[world_id];
}

bool GameDataFile::isEmpty() const {
    return mIsPlayDemoOpening;
}

static void clearShineInfo(ShineInfo** shine_list) {
    for (s32 i = 0; i < 1024; i++)
        shine_list[i]->clear();
}

void GameDataFile::initializeData() {
    initializeCheckpointTable();

    mChangeStageInfo->init();
    mSphinxQuizData->init();
    _9f8 = 0;
    mIsKidsMode = false;
    generateSaveDataIdForPrepo();
    mPlayerHitPointData->setKidsModeFlag(mIsKidsMode);
    mPlayerHitPointData->init();
    mStartShineIndex = -1;
    resetMapIcon();

    clearShineInfo(mGotShine.begin());
    clearShineInfo(mGotGrandShine.begin());
    mLatestGetShineInfo->clear();

    _458 = false;
    _a09 = false;

    for (s32 i = 0; i < 512; i++)
        mStartedObj[i].clear();
    for (s32 i = 0; i < 256; i++) {
        mSaveObjS32[i].objInfo.clear();
        mSaveObjS32[i].value = 0;
    }
    for (s32 i = 0; i < 16; i++) {
        GrowFlower* flower = &mGrowFlower[i];
        flower->potObj.clear();
        flower->seedObj.clear();
        flower->time = 0;
        flower->growLevel = 0;
    }
    for (s32 i = 0; i < 320; i++)
        mGotCheckpoint[i].clear();
    for (s32 i = 0; i < 1024; i++)
        mHintList[i].clear();
    for (s32 i = 0; i < 20; i++) {
        mUnlockedWorldId[i] = -1;
        mMainScenarioNo[i] = -1;
        mShineNum[i] = 0;
        mShopShineNum[i] = 0;
        mPayShineNum[i] = 0;
        mCoinCollectGotNum[i] = 0;
        mUseCoinCollectNum[i] = 0;
    }
    mUnlockedWorldId[0] = 0;
    mIsPayShineAllInAllWorld = false;
    for (s32 i = 0; i < mGameDataHolder->getWorldList()->getWorldNum(); i++)
        mScenarioNo[i] = 1;
    mPlayTimeTotal = 0;

    for (s32 i = 0; i < 64; i++) {
        mItemCap[i].clear();
        mItemCloth[i].clear();
        mItemSticker[i].clear();
        mItemGift[i].clear();
    }
    buyDefaultItem();

    for (s32 i = 0; i < 160; i++)
        mHackDictionary[i].clear();
    mIsRideSphinx = false;
    mIsRideMotorcycle = false;
    for (s32 i = 0; i < SessionMusicianType::size(); i++)
        mIsExistSessionMember[i] = false;
    mSessionEventProgress = SessionEventProgress::Entry;
    for (s32 i = 0; i < mRaceRecord.size(); i++)
        mRaceRecord[i]->init();
    mRaceLoseCountLv1 = 0;
    mRaceLoseCountLv2 = 0;
    mJumpingRopeBestCount = 0;
    mJumpingRopeDayCount = 0;
    mIsExistRecordJumpingRope = false;
    mIsExistRecordInDayJumpingRope = false;
    mVolleyballBestCount = 0;
    mVolleyballDayCount = 0;
    mIsExistRecordVolleyball = false;
    mIsExistRecordInDayVolleyball = false;

    for (s32 i = 0; i < 4; i++) {
        ShopNpcInfo* info = &mShopNpcInfo[i];
        info->trans.set(sead::Vector3f::zero);
        info->name.clear();
        info->type = 0;
    }

    mShopTalkData->init();
    for (s32 i = 0; i < 4; i++)
        mMiniGameInfo[i].clear();

    mIsShowExplainCheckpointFlag = false;
    mTotalPayShineNumForDisp = 0;
    mCurrentWorldIdForDisp = 0;
    mRaceType = RaceType_None;
    mRaceResult = RaceResult_None;
    mLastRaceRanking = 0;
    mKeyNum = 0;
    mOpenDoorLockNum = 0;
    mCoinNum = 0;
    mTotalCoinNum = 0;
    mPlayerJumpCount = 0;
    mPlayerThrowCapCount = 0;
    wearDefault();
    mIsCostumeRandom = false;
    mIsCapRandom = false;
    mPlayerStartId.clear();
    mPlayerStartIdForSave.clear();
    _160.clear();
    mCheckpointName.clear();
    _290.clear();
    mPrevStageName.clear();
    mCurrentStageName.clear();
    mNextStageName.clear();
    mIsStageHakoniwa = false;
    mGotCheckpointNum = 0;
    _908.clear();
    mIsEnterStageFirst = true;
    _459 = false;
    mIsPlayDemoReturnToHome = false;
    mIsPlayDemoAwardSpecial = false;
    mIsPlayDemoOpening = true;
    mIsWarpCheckpoint = false;
    mIsMeetCap = false;
    mIsEnableCap = true;
    mIsUseMissRestartInfo = false;
    mIsStartKoopaCapture = false;
    mIsMissEndPrevStage = false;
    mSaveTime = 0;
    mSaveTimeForDisp = 0;
    // TODO maybe reorder the following two lines?
    mIsEnableCapMessageLifeOneKidsMode = true;
    mIsKoopaLv3 = false;
    mIsPayCoinToSphinx = false;
    for (s32 i = 0; i < 20; i++)
        mIsTalkLocalLanguage[i] = false;
    for (s32 i = 0; i < 20; i++)
        mIsWorldWarpHoleThrough[i] = false;
    mIsHintNpcFirstTalk = false;
    mIsKinopioBrigadeNpcFirstTalk = false;
    mCapStatusForJango = CapStatus::None;
    mCapStatusForJangoSubArea = CapStatus::None;
    mJangoCount = 0;
    mTimeBalloonSaveData->init();
    mWorldTravelingStatus.clear();
    mIsStartWorldTravelingPeach = false;
    mIsPlayAlreadyWorldWarp = false;
    mIsTalkFirstAmiiboNpc = true;
    mIsUnlockAchievement = false;

    mWorldWarpTalkData->init();
    mVisitStageData->init();
    mGameProgressData->init();
    mMoonRockData->init();
    mBossSaveData->init();
    mAchievementSaveData->init();
    mSearchAmiiboDataTable->init();
    mNetworkUploadFlag->init();
    mSequenceDemoSkipData->init();
    mHintPhotoData->init();

    mCurrentWorldId = -1;
    mCurrentWorldIdForWrite = -1;
    mUnlockedWorldNum = 1;
    mNextWorldId = 0;
    mPrevWorldId = 0;
    for (s32 i = 0; i < 20; i++) {
        mTotalShineNum[i] = 0;
        mTotalMoonRockShineNum[i] = 0;
        mWorldMapIndex[i] = -1;
        mWorldWarpIndex[i] = -1;
    }
    mTotalAchievementNum = 0;
    mIsPlayDemoPlayerDownForBattleKoopaAfter = true;
    mScenarioNoPlacement = -1;
    mScenarioNoOverride = -1;
    _a2c = false;
    mWorldMapIndex[0] = 0;
    mWorldWarpIndex[0] = 0;
    mWorldMapNum = 1;
    for (s32 i = 0; i < 256; i++)
        mFlagTalkMessage[i] = false;
    mIsTalkKakku = false;
    mIsTalkWorldTravelingPeach = false;
    mIsTalkCollectBgmNpc = false;
    mTokimekiMayorNpcFavorabilityRating = 0;
    mIsFirstNetwork = true;

    initializeHintList();
    initializeCoinCollectList();

    for (s32 i = 0; i < mCollectBgmList.size(); i++)
        mCollectBgmList[i]->isCollected = false;
}

void GameDataFile::initializeCheckpointTable() {
    for (s32 i = 0; i < sNumWorlds; i++) {
        for (s32 j = 0; j < 16; j++) {
            CheckpointInfo* info = &mCheckpointTable[i][j];
            info->objInfo.clear();
            info->scenarios = 0;
            info->trans.set(sead::Vector3f::zero);
            info->isGet = false;
        }
    }
    for (s32 i = 0; i < mGameDataHolder->getWorldList()->getWorldNum(); i++) {
        al::StringTmp<128> filename;
        const char* stage_name = mGameDataHolder->getWorldList()->getMainStageName(i);
        s32 count = 0;
        for (s32 j = 1; j < 20; j++) {
            filename.format("FlagList_%s_%d", stage_name, j);
            const u8* data =
                al::tryGetBymlFromArcName("SystemData/CheckpointFlagInfo", filename.cstr());
            if (!data)
                continue;
            al::ByamlIter iter{data};
            iter.tryGetIterByKey(&iter, "FlagList");
            for (s32 k = 0, len = iter.getSize(); k < len; k++) {
                al::ByamlIter subiter;
                iter.tryGetIterByIndex(&subiter, k);
                const char* obj_id = al::getByamlKeyString(subiter, "FlagIdStr");
                if (CheckpointInfo* info =
                        tryFindCheckpointInfoImpl(mCheckpointTable, stage_name, obj_id)) {
                    info->scenarios.setBit(j);
                    continue;
                }
                mCheckpointTable[i][count].objInfo.stageName.format(stage_name);
                mCheckpointTable[i][count].objInfo.setObjId(obj_id);
                mCheckpointTable[i][count].scenarios.setBit(j);
                count++;
            }
        }
    }
}

void GameDataFile::generateSaveDataIdForPrepo() {
    mSaveDataIdForPrepo = rs::prepo::generateSaveDataId();
}

void GameDataFile::resetMapIcon() {
    mHomeTrans.set(sead::Vector3f::zero);
    mIsExistHome = false;
    mRaceStartNpcTrans.set(sead::Vector3f::zero);
    mIsExistRaceStartNpc = false;
    mRaceStartTrans.set(sead::Vector3f::zero);
    mIsExistRaceStart = false;
    mRaceGoalTrans.set(sead::Vector3f::zero);
    mIsExistRaceGoal = false;
    mHintNpcTrans.set(sead::Vector3f::zero);
    mIsExistHintNpc = false;
    _acc.set(sead::Vector3f::zero);
    _ad8 = false;
    mJangoTrans.set(sead::Vector3f::zero);
    mIsEnableJangoTrans = false;
    mAmiiboNpcTrans.set(sead::Vector3f::zero);
    mIsExistAmiiboNpc = false;
    mTimeBalloonNpcTrans.set(sead::Vector3f::zero);
    mIsExistTimeBalloonNpc = false;
    mPoetterTrans.set(sead::Vector3f::zero);
    mIsExistPoetter = false;
    mMoonRockTrans.set(sead::Vector3f::zero);
    mIsExistMoonRock = false;
    for (s32 i = 0; i < 4; i++)
        mMiniGameInfo[i].clear();
    for (s32 i = 0; i < 4; i++) {
        ShopNpcInfo* info = &mShopNpcInfo[i];
        info->trans.set(sead::Vector3f::zero);
        info->name.clear();
        info->type = 0;
    }
}

void GameDataFile::wearDefault() {
    mCurrentCostumeName.format("Mario");
    mCurrentCapName.format("Mario");
}

static s32 compareHintInfoByIdx(const GameDataFile::HintInfo* a, const GameDataFile::HintInfo* b) {
    return a->hintIdx == b->hintIdx ? 0 : a->hintIdx > b->hintIdx ? 1 : -1;
}

void GameDataFile::initializeHintList() {
    for (s32 i = 0; i < 20; i++) {
        mTotalShineNum[i] = 0;
        mTotalMoonRockShineNum[i] = 0;
    }
    s32 table_size = mHintTable.size();
    for (s32 i = 0; i < table_size; i++) {
        mHintTable[i]->list.clear();
        mHintTableByIdx[i]->list.clear();
    }
    mTotalAchievementNum = 0;
    s32 count = 0;
    for (s32 i = 0; i < mGameDataHolder->getWorldList()->getWorldNum(); i++) {
        al::StringTmp<128> filename;
        filename.format("ShineList_%sWorldHomeStage",
                        mGameDataHolder->getWorldList()->getWorldDevelopName(i));
        const u8* data = al::tryGetBymlFromArcName("SystemData/ShineInfo", filename.cstr());
        if (!data)
            continue;
        al::ByamlIter iter{data};
        iter.tryGetIterByKey(&iter, "ShineList");
        s32 len = iter.getSize();
        for (s32 j = 0; j < len; j++) {
            al::ByamlIter subiter;
            iter.tryGetIterByIndex(&subiter, j);
            sead::Vector3f trans = sead::Vector3f::zero;
            al::tryGetByamlV3f(&trans, subiter, "Trans");
            mHintList[count].stageName.format("%s", al::getByamlKeyString(subiter, "StageName"));
            mHintList[count].objId.format("%s", al::getByamlKeyString(subiter, "ObjId"));
            mHintList[count].scenarioName = al::getByamlKeyString(subiter, "ScenarioName");
            mHintList[count].trans.set(trans);
            mHintList[count].originalTrans.set(trans);
            mHintList[count].worldId = i;
            u32 progress_bit_flag = 0;
            al::tryGetByamlU32(&progress_bit_flag, subiter, "ProgressBitFlag");
            mHintList[count].progressBitFlag = progress_bit_flag;
            mHintList[count].isMoonRock = al::tryGetByamlKeyBoolOrFalse(subiter, "IsMoonRock");
            if (i == GameDataFunction::getWorldIndexPeach()) {
                mHintList[count].isMoonRock = false;
            } else if (mHintList[count].progressBitFlag.isOnBit(
                           mGameDataHolder->getWorldList()->getMoonRockScenarioNo(i) - 1) &&
                       mHintList[count].progressBitFlag.countOnBit() == 1) {
                mHintList[count].isMoonRock = true;
            }
            mHintList[count].isGet = false;
            mHintList[count].getTime = 0;
            mHintList[count].isAchievement =
                al::tryGetByamlKeyBoolOrFalse(subiter, "IsAchievement");
            mHintList[count].isGrand = al::tryGetByamlKeyBoolOrFalse(subiter, "IsGrand");
            mHintList[count].uniqueId = al::getByamlKeyInt(subiter, "UniqueId");
            al::tryGetByamlS32(&mHintList[count].mainScenarioNo, subiter, "MainScenarioNo");
            const char* object_name = al::tryGetByamlKeyStringOrNULL(subiter, "ObjectName");
            mHintList[count].objectName.format("%s", object_name);
            mHintList[count].isShop =
                object_name && al::isEqualSubString(object_name, "ショップ店員");
            mHintList[count].status = AchievementStatus_None;
            if (mHintList[count].isAchievement)
                mTotalAchievementNum++;
            else if (mHintList[count].isMoonRock)
                mTotalMoonRockShineNum[i]++;
            else
                mTotalShineNum[i]++;
            if (const char* str = al::tryGetByamlKeyStringOrNULL(subiter, "OptionalId"))
                mHintList[count].optionalId.format("%s", str);
            if (al::isEqualString("CityWorld2DSign004Zone", mHintList[count].stageName.cstr()) &&
                al::isEqualString("obj346", mHintList[count].objId.cstr())) {
                mHintList[count].progressBitFlag = 0b10011011100;
                mHintList[count].isEnableHintInCeremony = true;
            }
            mHintList[count].hintIdx = al::tryGetByamlKeyIntOrZero(subiter, "HintIdx");
            mHintTable[i]->list.pushBack(&mHintList[count]);
            mHintTableByIdx[i]->list.pushBack(&mHintList[count]);
            count++;
        }
    }
    while (count < 1024)
        mHintList[count++].clear();
    for (s32 i = 0; i < table_size; i++)
        mHintTableByIdx[i]->list.sort(compareHintInfoByIdx);
}

void GameDataFile::initializeCoinCollectList() {
    al::ByamlIter iter{al::getBymlFromArcName("SystemData/WorldList", "CoinCollectList")};
    al::ByamlIter table_iter;
    iter.tryGetIterByKey(&table_iter, "Table");
    s32 len = table_iter.getSize();
    s32 world_num = mGameDataHolder->getWorldList()->getWorldNum();
    for (s32 i = 0; i < mCoinCollectList.capacity(); i++)
        mCoinCollectList[i]->clear();
    for (s32 i = 0; i < world_num; i++)
        mCoinCollectTable[i]->list.clear();
    s32 count = 0;
    for (s32 i = 0; i < len; i++) {
        al::ByamlIter subiter;
        table_iter.tryGetIterByIndex(&subiter, i);
        const char* stage_name = al::getByamlKeyString(subiter, "StageName");
        const char* obj_id = al::getByamlKeyString(subiter, "ObjId");
        s32 unique_id = al::tryGetByamlKeyIntOrZero(subiter, "UniqueId");
        s32 world_id = mGameDataHolder->getWorldList()->tryFindWorldIndexByStageName(stage_name);
        if (world_id == -1)
            continue;
        CoinCollectInfo* info = mCoinCollectList[count];
        info->stageName.format("%s", stage_name);
        info->objId.format("%s", obj_id);
        info->uniqueId = unique_id;
        info->worldId = world_id;
        mCoinCollectTable[world_id]->list.pushBack(info);
        count++;
    }
}

void GameDataFile::resetTempData() {}

bool GameDataFile::isGameClear() const {
    return isUnlockedWorld(GameDataFunction::getWorldIndexPeach()) &&
           isAlreadyGoWorld(GameDataFunction::getWorldIndexPeach());
}

bool GameDataFile::isUnlockedWorld(s32 world_id) const {
    return mGameProgressData->isUnlockWorld(world_id);
}

bool GameDataFile::isAlreadyGoWorld(s32 world_id) const {
    return mGameProgressData->isAlreadyGoWorld(world_id);
}

void GameDataFile::updateWorldMapIndex() {
    mWorldMapNum = 0;
    for (s32 i = 0; i < mWorldMapIndex.size(); i++)
        mWorldMapIndex[i] = -1;
    bool has_peach_world = isUnlockedWorld(GameDataFunction::getWorldIndexPeach());
    if (has_peach_world) {
        mWorldMapIndex[0] = GameDataFunction::getWorldIndexPeach();
        mWorldMapNum++;
    }
    for (s32 i = 0; i < mGameDataHolder->getWorldList()->getWorldNum(); i++) {
        s32 index = mUnlockedWorldId[i];
        if (i == GameDataFunction::getWorldIndexPeach() || index < 0)
            continue;
        if (has_peach_world && index < GameDataFunction::getWorldIndexPeach())
            index++;
        mWorldMapIndex[index] = i;
        mWorldMapNum++;
    }
}

void GameDataFile::updateWorldWarpIndex() {
    mWorldWarpNum = 0;
    for (s32 i = 0; i < mWorldWarpIndex.size(); i++)
        mWorldWarpIndex[i] = -1;
    for (s32 i = 0; i < mGameDataHolder->getWorldList()->getWorldNum(); i++) {
        if (mUnlockedWorldId[i] < 0)
            continue;
        mWorldWarpIndex[mUnlockedWorldId[i]] = i;
        mWorldWarpNum++;
    }
}

void GameDataFile::addPlayTime(s32 time, const al::IUseSceneObjHolder*) {
    mPlayTimeTotal += time;
}

s64 GameDataFile::getPlayTimeTotal() const {
    return mPlayTimeTotal;
}

void GameDataFile::updateSaveTime() {
    sead::DateTime time;
    time.setNow();
    mSaveTime = time.getUnixTime();
}

void GameDataFile::updateSaveTimeForDisp() {
    mSaveTimeForDisp = mSaveTime;
}

void GameDataFile::updateSaveInfoForDisp() {
    mTotalPayShineNumForDisp = 0;
    for (s32 i = 0; i < 20; i++)
        mTotalPayShineNumForDisp =
            sead::Mathi::clampMax(mTotalPayShineNumForDisp + mPayShineNum[i], 999);
    mCurrentWorldIdForDisp = mCurrentWorldIdForWrite;
}

u64 GameDataFile::getLastUpdateTime() const {
    return mSaveTimeForDisp;
}

void GameDataFile::generateSaveDataIdForPrepoForWrite() {
    mSaveDataIdForPrepoForWrite = rs::prepo::generateSaveDataId();
}

void GameDataFile::resetSaveDataIdForPrepoForWrite() {
    mSaveDataIdForPrepoForWrite = mSaveDataIdForPrepo;
}

void GameDataFile::startStage(const char* stage_name, s32 scenario_no) {
    mGameDataHolder->validateSaveForMoonGet();
    mIsUseMissRestartInfo = false;
    mIsEnableCap = true;
    if (al::isEqualString(stage_name, "MoonWorldKoopa2Stage"))
        mIsKoopaLv3 = true;
    mGameDataHolder->getSequenceInfo()->resetSceneStatus();
    if (rs::isInvalidSaveStage(stage_name))
        mGameDataHolder->getSequenceInfo()->setSceneStatusInvalidSave();
    if (!mIsMissEndPrevStage)
        mCapStatusForJangoSubArea = CapStatus::None;
    if (mIsPlayDemoReturnToHome) {
        mPlayerStartId.clear();
        _160.clear();
        mCheckpointName.clear();
    }
    mScenarioNoPlacement = scenario_no;
    mCurrentStageName.format("%s", stage_name);
    if (_290.isEmpty())
        _290.format("%s", stage_name);
    if (!isRaceStart())
        mRaceRivalLevel = -1;
    if (mIsMissEndPrevStage && mCurrentWorldId == GameDataFunction::getWorldIndexSea() &&
        al::isEqualString(mCurrentStageName.cstr(),
                          mGameDataHolder->getWorldList()->getMainStageName(mCurrentWorldId)) &&
        mScenarioNoPlacement == 1 && mMainScenarioNo[mCurrentWorldId] == 1) {
        mPlayerStartId.format("GiantWanderBoss");
        mCheckpointName.clear();
        _908.clear();
    }
    s32 world_id = mGameDataHolder->getWorldList()->tryFindWorldIndexByMainStageName(stage_name);
    if (mIsPlayDemoWorldWarpHole)
        world_id = mWarpHoleWorldId;
    if (world_id == GameDataFunction::getWorldIndexWaterfall() &&
        (mGameDataHolder->getWorldList()->isEqualAfterEndingScenarioNo(world_id, scenario_no) ||
         mGameDataHolder->getWorldList()->isEqualMoonRockScenarioNo(world_id, scenario_no)))
        mGameProgressData->setAlreadyGoWorld(GameDataFunction::getWorldIndexSand());
    if (world_id != -1) {
        mCurrentWorldId = world_id;
        if (mGameProgressData->isUnlockWorld(world_id))
            mCurrentWorldIdForWrite = world_id;
        if (GameDataFunction::isCityWorldCeremonyAll(world_id, mScenarioNoPlacement)) {
            mScenarioNoOverride = mScenarioNoPlacement;
            if (GameDataFunction::isCityWorldCeremonyAgain(world_id, mScenarioNoPlacement))
                _a2c = true;
        } else {
            mScenarioNoOverride = -1;
        }
        mGameProgressData->checkAndChangeCorrectStatus(world_id, scenario_no);
        if (mGameProgressData->isUnlockWorld(world_id)) {
            if (checkIsHomeStage(stage_name) && scenario_no != -1 && !_a2c) {
                if (scenario_no != mScenarioNo[getCurrentWorldIdNoDevelop()]) {
                    mGameDataHolder->resetTempSaveDataInSameScenario();
                    mGameDataHolder->setRequireSave();
                }
                mScenarioNo[getCurrentWorldIdNoDevelop()] = scenario_no;
            }
            if (world_id > 0)
                mScenarioNo[0] = sead::Mathi::max(mScenarioNo[0], 2);
            if (world_id > GameDataFunction::getWorldIndexWaterfall()) {
                switch (mScenarioNo[GameDataFunction::getWorldIndexWaterfall()]) {
                case 1:
                case 2:
                    mScenarioNo[GameDataFunction::getWorldIndexWaterfall()] = 7;
                    break;
                }
            }
            if (world_id > GameDataFunction::getWorldIndexClash()) {
                switch (mScenarioNo[GameDataFunction::getWorldIndexClash()]) {
                case 1:
                    mScenarioNo[GameDataFunction::getWorldIndexClash()] =
                        sead::Mathi::max(mScenarioNo[GameDataFunction::getWorldIndexClash()], 2);
                    mCapStatusForJango = CapStatus::None;
                    break;
                }
            }
            if (world_id > GameDataFunction::getWorldIndexCloud()) {
                mScenarioNo[GameDataFunction::getWorldIndexCloud()] =
                    sead::Mathi::max(mScenarioNo[GameDataFunction::getWorldIndexCloud()], 2);
            }
            if (world_id > GameDataFunction::getWorldIndexMoon()) {
                mScenarioNo[GameDataFunction::getWorldIndexMoon()] =
                    sead::Mathi::max(mScenarioNo[GameDataFunction::getWorldIndexMoon()], 2);
            }
            mMainScenarioNo[mCurrentWorldId] = sead::Mathi::max(
                mMainScenarioNo[mCurrentWorldId], mGameDataHolder->getWorldList()->getMainQuestMin(
                                                      mCurrentWorldId, mScenarioNoPlacement));
            mGameProgressData->setAlreadyGoWorld(mCurrentWorldId);
            if (world_id >= GameDataFunction::getWorldIndexSand())
                mGameProgressData->launchHome();
            if (world_id == GameDataFunction::getWorldIndexWaterfall() && scenario_no == 7)
                mGameProgressData->launchHome();
            if (isGameClear())
                setGameClear();
        }
    }
    mIsPlayDemoOpening = false;
    mIsStageHakoniwa = false;
    mStartShineIndex = -1;
    for (s32 i = 0; i < 1024; i++) {
        if (mHintList[i].stageName.isEmpty())
            break;
        if (!al::isEqualSubString(mHintList[i].objectName.cstr(), "育つ草"))
            setOriginalHintTrans(i);
    }
    if (mCurrentWorldId == GameDataFunction::getWorldIndexBoss() &&
        mScenarioNo[mCurrentWorldId] == 1 && !mGameProgressData->isRepairHomeByCrashedBoss())
        mGameProgressData->bossAttackHome();
    if (mCurrentWorldId == GameDataFunction::getWorldIndexClash() &&
        mScenarioNo[mCurrentWorldId] == 1 && !mGameProgressData->isRepairHome())
        mGameProgressData->crashHome();
    if (mCurrentWorldId == GameDataFunction::getWorldIndexClash() &&
        mScenarioNo[mCurrentWorldId] == 2)
        mGameProgressData->repairHome();
    if (mMainScenarioNo[GameDataFunction::getWorldIndexCity()] <= 1)
        mMainScenarioNo[GameDataFunction::getWorldIndexCity()] = 0;
    if (mChangeStageInfo->isSubScenarioTypeLifeRecover())
        mPlayerHitPointData->forceNormalMode();
    else if (mPlayerHitPointData->isForceNormalMode())
        mPlayerHitPointData->endForceNormalMode();
    if (mChangeStageInfo->isSubScenarioTypeResetMiniGame())
        mGameDataHolder->resetMiniGameData();
    mNextStageName.format("%s", stage_name);
}

bool GameDataFile::isRaceStart() const {
    return mRaceType != RaceType_None;
}

bool GameDataFile::checkIsHomeStage(const char* stage_name) const {
    return mGameDataHolder->getWorldList()->tryFindWorldIndexByMainStageName(stage_name) != -1;
}

void GameDataFile::setGameClear() {
    mMainScenarioNo[GameDataFunction::getWorldIndexMoon()] = 0;
    mScenarioNo[GameDataFunction::getWorldIndexPeach()] =
        sead::Mathi::max(mScenarioNo[GameDataFunction::getWorldIndexPeach()], 2);
}

PlayerHitPointData* GameDataFile::getPlayerHitPointData() const {
    return mPlayerHitPointData;
}

void GameDataFile::startDemoStage(const char* stage_name) {
    mCurrentStageName.format("%s", stage_name);
}

void GameDataFile::changeNextStage(const ChangeStageInfo* info, s32 race_type) {
    mIsStartKoopaCapture = false;
    const char* stage_name = info->getStageName();
    if (al::isEqualString(stage_name, "CurrentWorldHome"))
        stage_name =
            mGameDataHolder->getWorldList()->getMainStageName(getCurrentWorldIdNoDevelop());
    const char* start_id = info->getChangeStageId();
    mGameDataHolder->tryFindLinkDestStageInfo(&stage_name, &start_id, mCurrentStageName.cstr(),
                                              info->getChangeStageId());
    bool is_return = info->isReturn();
    s32 scenario_no = info->getScenarioNo();
    const char* placement_string = info->getPlacementString();
    ChangeStageInfo::SubScenarioType sub_scenario_type = info->getSubScenarioType();
    if (_458 || (sub_scenario_type == ChangeStageInfo::NO_SUB_SCENARIO && _a2c))
        mGameDataHolder->resetMiniGameData();
    bool has_scenario_no;
    if (al::isEqualString(start_id, "back_from_ceremony")) {
        mScenarioNoOverride = -1;
        has_scenario_no = scenario_no != -1;
    } else {
        if (mScenarioNoOverride != -1 && scenario_no != -1)
            mScenarioNoOverride = -1;
        has_scenario_no = scenario_no != -1;
    }
    if (_458) {
        _458 = false;
        returnPrevStage();
        return;
    }
    if (sub_scenario_type != ChangeStageInfo::NO_SUB_SCENARIO &&
        al::isEqualString(stage_name, "CityWorldHomeStage") &&
        GameDataFunction::isCityWorldCeremonyAgain(GameDataFunction::getWorldIndexCity(),
                                                   scenario_no))
        mGameDataHolder->resetScenarioStartCamera();
    mChangeStageInfo->copy(*info);
    mRaceType = static_cast<RaceType>(race_type);
    if (race_type != RaceType_None) {
        mRaceResult = RaceResult_None;
        mLastRaceRanking = 0;
    }
    _a2c = sub_scenario_type != ChangeStageInfo::NO_SUB_SCENARIO;
    mScenarioNoPlacement = scenario_no;
    bool flag = checkIsHomeStage(stage_name) && has_scenario_no;
    if (flag && sub_scenario_type == ChangeStageInfo::NO_SUB_SCENARIO) {
        s32 world_id =
            mGameDataHolder->getWorldList()->tryFindWorldIndexByMainStageName(stage_name);
        if (world_id == -1)
            world_id = mCurrentWorldId;
        world_id = sead::Mathi::max(world_id, 0);
        if (mScenarioNo[world_id] != scenario_no) {
            mGameDataHolder->resetTempSaveDataInSameScenario();
            mBossSaveData->resetLv3Data();
            mGameDataHolder->setRequireSave();
        }
        mScenarioNo[world_id] = scenario_no;
        if (isGameClear() &&
            mGameDataHolder->getWorldList()->isEqualClearMainScenarioNo(world_id, scenario_no))
            mScenarioNo[world_id] =
                mGameDataHolder->getWorldList()->getAfterEndingScenarioNo(world_id);
        resetMapIcon();
    }
    if (is_return && placement_string)
        _160.format("%s", placement_string);
    mNextStageName.format("%s", stage_name);
    if (start_id)
        mPlayerStartId.format("%s", start_id);
    else
        mPlayerStartId.clear();
    mCheckpointName.clear();
    _908.clear();
    _458 = is_return;
}

void GameDataFile::changeNextStageWithDemoWorldWarp(const char* stage_name) {
    mScenarioNoOverride = -1;
    mBossSaveData->resetLv3Data();
    if (mGameProgressData->isFindKoopa())
        stage_name = mGameDataHolder->getWorldList()->getMainStageName(
            GameDataFunction::getWorldIndexCloud());
    if (mGameProgressData->isBossAttackedHome())
        stage_name = mGameDataHolder->getWorldList()->getMainStageName(
            GameDataFunction::getWorldIndexBoss());
    for (s32 i = 0; i < mGameDataHolder->getWorldList()->getWorldNum(); i++)
        if (i != GameDataFunction::getWorldIndexPeach() &&
            mGameDataHolder->getWorldList()->isEqualClearMainScenarioNo(i, mScenarioNo[i]) &&
            isGameClear())
            mScenarioNo[i] = mGameDataHolder->getWorldList()->getAfterEndingScenarioNo(i);
    mPlayerStartIdForSave.clear();
    mPrevWorldId = mCurrentWorldId;
    ChangeStageInfo info{mGameDataHolder, "", stage_name};
    changeNextStage(&info, 0);
    _9f8 = 1;
    mNextWorldId = GameDataFunction::calcNextWorldId(mGameDataHolder);
    resetMapIcon();
}

void GameDataFile::changeNextStageWithWorldWarpHole(const char* name) {
    mScenarioNoOverride = -1;
    mBossSaveData->resetLv3Data();
    al::StringTmp<128> stage_name;
    al::StringTmp<128> label;
    mGameDataHolder->calcWorldWarpHoleLabelAndStageName(&label, &stage_name, name,
                                                        getCurrentWorldIdNoDevelop());
    mPlayerStartIdForSave.clear();
    mPrevWorldId = mCurrentWorldId;
    ChangeStageInfo info{mGameDataHolder, label.cstr(), stage_name.cstr()};
    changeNextStage(&info, 0);
    mIsPlayDemoWorldWarpHole = true;
    if (al::isEqualString(name, "Go")) {
        _9ff = true;
        mIsWorldWarpHoleThrough[mCurrentWorldId] = true;
    } else {
        _9ff = false;
        s32 world_id =
            mGameDataHolder->getWorldList()->tryFindWorldIndexByStageName(stage_name.cstr());
        if (world_id >= 0)
            mIsWorldWarpHoleThrough[world_id] = true;
    }
    resetMapIcon();
}

void GameDataFile::returnPrevStage() {
    if (mPrevStageName.isEmpty()) {
        restartStage();
        return;
    }
    al::StringTmp<128> stage_name;
    al::StringTmp<128> start_id;
    stage_name.format("%s", mPrevStageName.cstr());
    if (mPlayerStartId.isEmpty() && !_160.isEmpty())
        start_id.format("%s", _160.cstr());
    else
        start_id.format("%s", mPlayerStartId.cstr());
    s32 scenario_no = -1;
    if (_a2c) {
        _a2c = false;
        scenario_no = calcNextScenarioNo();
    }
    ChangeStageInfo info{mGameDataHolder, start_id.cstr(), stage_name.cstr(), false, scenario_no};
    changeNextStage(&info, 0);
}

void GameDataFile::restartStage() {
    _a09 = false;
    if (mCheckpointName.isEmpty())
        return;
    mPlayerStartId.clear();
    _160.clear();
    mNextStageName.format("%s", _290.cstr());
}

s32 GameDataFile::calcNextScenarioNo() const {
    if (mNextStageName.isEmpty())
        return -1;
    if (mScenarioNoOverride != -1)
        return mScenarioNoOverride;
    if (!mIsWarpCheckpoint && mScenarioNoPlacement != -1)
        return mScenarioNoPlacement;
    s32 world_id =
        mGameDataHolder->getWorldList()->tryFindWorldIndexByMainStageName(mNextStageName.cstr());
    if (world_id != -1) {
        if (world_id == GameDataFunction::getWorldIndexCity() && !isUnlockedWorld(world_id))
            return 10;
        if (world_id == GameDataFunction::getWorldIndexLava() && !isUnlockedWorld(world_id))
            return 10;
        return mScenarioNo[world_id];
    }
    return mScenarioNo[getCurrentWorldIdNoDevelop()];
}

const char* GameDataFile::getStageNameCurrent() const {
    return mCurrentStageName.cstr();
}

const char* GameDataFile::tryGetStageNameCurrent() const {
    if (mCurrentStageName.isEmpty())
        return nullptr;
    const char* str = mCurrentStageName.cstr();
    if (al::isEqualString(str, ""))
        return nullptr;
    return str;
}

const char* GameDataFile::getStageNameNext() const {
    if (mNextStageName.isEmpty())
        return nullptr;
    const char* str = mNextStageName.cstr();
    if (al::isEqualString(str, ""))
        return nullptr;
    return str;
}

void GameDataFile::changeWipeType(const char* type) {
    mChangeStageInfo->setWipeType(type);
}

void GameDataFile::setActivateHome() {
    mGameProgressData->activateHome();
}

bool GameDataFile::isGoToCeremonyFromInsideHomeShip() const {
    // TODO cleanup
    if (al::isEqualString(getStageNameCurrent(), GameDataFunction::getHomeShipStageName()) &&
        mScenarioNoOverride != -1)
        return true;
    return false;
}

const char* GameDataFile::getPlayerStartId() const {
    if (mPlayerStartId.isEmpty())
        return nullptr;
    const char* str = mPlayerStartId.cstr();
    if (al::isEqualString(str, ""))
        return nullptr;
    return str;
}

static UniqObjInfo* addGotCheckpoint(UniqObjInfo* list, const char* stage_name,
                                     const char* obj_id) {
    for (s32 i = 0; i < 320; i++) {
        if (list[i].stageName.isEmpty() && list[i].objId.isEmpty())
            return &list[i];
        if (al::isEqualString(list[i].getStageName(), stage_name) &&
            al::isEqualString(list[i].getObjId(), obj_id))
            break;
    }
    return nullptr;
}

void GameDataFile::setCheckpointId(const al::PlacementId* placement_id) {
    al::StringTmp<128> obj_id;
    placement_id->makeString(&obj_id);
    if (CheckpointInfo* info =
            tryFindCheckpointInfoImpl(mCheckpointTable, mCurrentStageName.cstr(), obj_id.cstr())) {
        info->isGet = true;
        if (UniqObjInfo* got_info = addGotCheckpoint(
                mGotCheckpoint.begin(), info->objInfo.getStageName(), info->objInfo.getObjId())) {
            got_info->setStageName(info->objInfo.getStageName());
            got_info->objId.format("%s", info->objInfo.getObjId());
            mGotCheckpointNum++;
        }
    }
    al::copyString(mCheckpointName.getBuffer(), obj_id.cstr(), 128);
    _290.format("%s", mCurrentStageName.cstr());
    _908.clear();
    mPlayerStartId.clear();
    _160.clear();
}

void GameDataFile::setRestartPointId(const al::PlacementId* placement_id) {
    _160.clear();
    mPlayerStartId.clear();
    placement_id->makeString(&_908);
}

void GameDataFile::clearStartId() {
    _160.clear();
    mPlayerStartId.clear();
    _908.clear();
    mCheckpointName.clear();
    _290.clear();
}

const char* GameDataFile::tryGetRestartPointIdString() const {
    if (!_908.isEmpty()) {
        const char* str = _908.cstr();
        if (al::isEqualString(str, ""))
            return nullptr;
        return str;
    }
    if (!mCheckpointName.isEmpty()) {
        const char* str = mCheckpointName.cstr();
        if (al::isEqualString(str, ""))
            return nullptr;
        return str;
    }
    return nullptr;
}

void GameDataFile::endStage() {
    _a09 = false;
    mPrevStageName.format("%s", mCurrentStageName.cstr());
    mCheckpointName.clear();
}

void GameDataFile::missAndRestartStage() {
    mIsEnableCapMessageLifeOneKidsMode = true;
    mBossSaveData->resetLv3Data();
    if (mIsUseMissRestartInfo) {
        mGameDataHolder->resetDeadPlayerCoinIdx();
        mGameDataHolder->changeNextStage(mMissRestartInfo);
        mIsMissEndPrevStage = true;
    } else {
        mIsMissEndPrevStage = true;
        restartStage();
    }
}

void GameDataFile::setMissRestartInfo(const al::PlacementInfo& info) {
    mMissRestartInfo->init(info, mGameDataHolder);
    mIsUseMissRestartInfo = true;
}

bool GameDataFile::isUseMissRestartInfo() const {
    return mIsUseMissRestartInfo;
}

bool GameDataFile::isFirstTimeNextWorld() const {
    return mGameProgressData->isFirstTimeGoWorld(mNextWorldId);
}

void GameDataFile::getAchievement(const char* name) {
    mAchievementSaveData->getAchievement(name);
    mShineNum[GameDataFunction::getWorldIndexPeach()]++;
    clampCount(&mShineNum[GameDataFunction::getWorldIndexPeach()]);
}

static bool contains(ShineInfo** shine_list, const ShineInfo* shine_info) {
    for (s32 i = 0; i < 1024; i++) {
        if (shine_list[i]->isEmpty())
            break;
        if (shine_list[i]->isEqual(shine_info))
            return true;
    }
    return false;
}

bool GameDataFile::isGotShine(const ShineInfo* info) const {
    if (contains(mGotShine.begin(), info) || contains(mGotGrandShine.begin(), info))
        return true;
    for (s32 i = 0; i < 1024; i++)
        if (isEqual(&mHintList[i], info))
            return mHintList[i].isGet;
    return false;
}

bool GameDataFile::isGotShine(s32 index) const {
    if (index < 0 || index >= 1024)
        return false;
    return mHintList[index].isGet;
}

bool GameDataFile::checkGotShine(const char* obj_id) const {
    for (s32 i = 0; i < 1024; i++)
        if (mHintList[i].isGet && al::isEqualString(mHintList[i].objId.cstr(), obj_id))
            return true;
    return false;
}

// TODO too hacky
static s32 func1(const GameDataFile::HintInfo* hint_info, const ShineInfo* shine_info) {
    if (al::isEqualString(hint_info->stageName.cstr(), shine_info->getStageName()) &&
        al::isEqualString(hint_info->objId.cstr(), shine_info->getObjId()))
        return false;
    return true;
}

void GameDataFile::setGotShine(const ShineInfo* info) {
    ShineInfo** list =
        info->getType() == ShineInfo::Type_GrandShine ? mGotGrandShine.begin() : mGotShine.begin();
    for (s32 i = 0; i < 1024; i++) {
        if (list[i]->isEmpty()) {
            mLatestGetShineInfo->copyShineInfo(info);
            bool has_hint = false;
            s32 hint_index = -1;
            for (s32 j = 0; j < 1024; j++) {
                if (!func1(&mHintList[j], info)) {
                    has_hint = true;
                    hint_index = j;
                }
            }
            if (isGotShine(info)) {
                if (info->isShop()) {
                    mShineNum[getCurrentWorldIdNoDevelop()]++;
                    mShopShineNum[getCurrentWorldIdNoDevelop()]++;
                    clampCount(&mShineNum[getCurrentWorldIdNoDevelop()]);
                    clampCount(&mShopShineNum[getCurrentWorldIdNoDevelop()]);
                }
                mLatestGetShineInfo->setGetTime(mHintList[hint_index].getTime);
                _459 = false;
                return;
            }
            list[i]->copyShineInfo(info);
            if (!has_hint) {
                if (list == mGotGrandShine.begin()) {
                    mShineNum[getCurrentWorldIdNoDevelop()] += 3;
                    if (mCurrentWorldId != GameDataFunction::getWorldIndexPeach())
                        mIsPlayDemoReturnToHome = true;
                } else {
                    mShineNum[getCurrentWorldIdNoDevelop()]++;
                }
                break;
            }
            if (mHintList[hint_index].isGrand) {
                clampCount(&mShineNum[mHintList[hint_index].worldId], 3);
                mShineNum[mHintList[hint_index].worldId] += 3;
                mGameDataHolder->invalidateSaveForMoonGet();
            } else {
                mShineNum[mHintList[hint_index].worldId]++;
                clampCount(&mShineNum[mHintList[hint_index].worldId]);
                if (info->isShop()) {
                    mShopShineNum[getCurrentWorldIdNoDevelop()]++;
                    clampCount(&mShopShineNum[getCurrentWorldIdNoDevelop()]);
                }
            }
            mHintList[hint_index].status = AchievementStatus_Got;
            mHintList[hint_index].isGet = true;
            sead::DateTime time;
            time.setNow();
            mHintList[hint_index].getTime = time.getUnixTime();
            mLatestGetShineInfo->setGetTime(time.getUnixTime());
            list[i]->setGetTime(time.getUnixTime());
            s32 main_scenario_no = mHintList[hint_index].mainScenarioNo;
            if (main_scenario_no < 0 ||
                mMainScenarioNo[getCurrentWorldIdNoDevelop()] > main_scenario_no)
                break;
            mMainScenarioNo[getCurrentWorldIdNoDevelop()] = main_scenario_no;
            if (info->getType() == ShineInfo::Type_GrandShine) {
                mPlayerStartId.clear();
                mPlayerStartIdForSave.clear();
            } else {
                mPlayerStartId.format("%s", mHintList[hint_index].objId.cstr());
                mPlayerStartIdForSave.format("%s", mHintList[hint_index].objId.cstr());
            }
            _160.clear();
            mCheckpointName.clear();
            _908.clear();
            _459 = true;
            break;
        }
    }
    if (rs::checkGetShineForWorldTravelingPeach(mGameDataHolder, "Moon"))
        mScenarioNo[GameDataFunction::getWorldIndexPeach()] =
            sead::Mathi::max(mScenarioNo[GameDataFunction::getWorldIndexPeach()], 3);
}

s32 GameDataFile::getShineNum() const {
    return getShineNum(mCurrentWorldId);
}

s32 GameDataFile::getShineNum(s32 world_id) const {
    return mShineNum[sead::Mathi::max(world_id, 0)];
}

s32 GameDataFile::getTotalShineNum() const {
    s32 count = 0;
    for (s32 i = 0; i < 20; i++)
        count += mShineNum[i];
    return count;
}

s32 GameDataFile::getTotalShopShineNum() const {
    s32 count = 0;
    for (s32 i = 0; i < mShopShineNum.size(); i++)
        count += mShopShineNum[i];
    return count;
}

bool GameDataFile::tryGetNextMainScenarioLabel(sead::BufferedSafeString* out_label,
                                               sead::BufferedSafeString* out_stage_name) const {
    s32 world_id = getCurrentWorldIdNoDevelop();
    s32 main_scenario_no = mMainScenarioNo[world_id];
    s32 scenario_no = mScenarioNo[world_id];
    if (GameDataFunction::isCityWorldCeremonyAgain(world_id, mScenarioNoPlacement)) {
        if (out_stage_name)
            out_stage_name->format("CityWorldHomeStage");
        if (out_label)
            out_label->format("Quest_08");
        return true;
    }
    const WorldList* world_list = mGameDataHolder->getWorldList();
    if (world_list->isEqualClearMainScenarioNo(world_id, scenario_no) ||
        world_list->isEqualAfterEndingScenarioNo(world_id, scenario_no) ||
        world_list->isEqualMoonRockScenarioNo(world_id, scenario_no)) {
        if (world_id == GameDataFunction::getWorldIndexPeach() && main_scenario_no == 0) {
            if (out_stage_name)
                out_stage_name->format("PeachWorldHomeStage");
            if (out_label)
                out_label->format("Quest_00");
            return true;
        }
        return false;
    }
    al::StringTmp<128> label;
    al::StringTmp<128> stage_name;
    label.format("Quest_%02d", main_scenario_no);
    stage_name.format("%s", mGameDataHolder->getWorldList()->getMainStageName(world_id));
    if (al::isExistLabelInStageMessage(mGameDataHolder, stage_name.cstr(), label.cstr())) {
        if (out_label)
            out_label->copy(label);
        if (out_stage_name)
            out_stage_name->copy(stage_name);
        return true;
    }
    const sead::PtrArray<HintInfo>& list = mHintTable[world_id]->list;
    for (s32 i = 0; i < list.size(); i++) {
        if (list[i]->mainScenarioNo == main_scenario_no) {
            if (out_label)
                out_label->format("ScenarioName_%s", list[i]->objId.cstr());
            if (out_stage_name)
                out_stage_name->copy(list[i]->stageName);
            return true;
        }
    }
    return false;
}

bool GameDataFile::tryGetNextMainScenarioPos(sead::Vector3f* out) const {
    const HintInfo* info = tryFindNextMainScenarioInfo();
    if (!info)
        return false;
    out->set(info->trans);
    return true;
}

const GameDataFile::HintInfo* GameDataFile::tryFindNextMainScenarioInfo() const {
    s32 world_id = getCurrentWorldIdNoDevelop();
    s32 next_main_scenario_no = mMainScenarioNo[world_id] + 1;
    for (s32 i = 0; i < 1024; i++)
        if (world_id == mHintList[i].worldId &&
            mHintList[i].mainScenarioNo == next_main_scenario_no)
            return &mHintList[i];
    return nullptr;
}

void GameDataFile::addPayShine(s32 count) {
    mPayShineNum[getCurrentWorldIdNoDevelop()] += count;
    if (GameDataFunction::calcIsGetShineAllInAllWorld(mGameDataHolder))
        mIsPayShineAllInAllWorld = true;
}

void GameDataFile::addPayShineCurrentAll() {
    for (s32 i = 0; i < mGameDataHolder->getWorldList()->getWorldNum(); i++)
        mPayShineNum[i] += sead::Mathi::clampMin(mShineNum[i] - getPayShineNum(i), 0);
    if (GameDataFunction::calcIsGetShineAllInAllWorld(mGameDataHolder))
        mIsPayShineAllInAllWorld = true;
}

s32 GameDataFile::getPayShineNum(s32 world_id) const {
    if (world_id == -1)
        world_id = getCurrentWorldIdNoDevelop();
    return mPayShineNum[world_id];
}

s32 GameDataFile::getTotalPayShineNum() const {
    s32 count = 0;
    for (s32 i = 0; i < 20; i++)
        count += mPayShineNum[i];
    return count;
}

bool GameDataFile::isPayShineAllInAllWorld() const {
    return mIsPayShineAllInAllWorld;
}

void GameDataFile::addKey(s32 count) {
    mKeyNum += count;
    mGameDataHolder->setRequireSave();
}

s32 GameDataFile::getKeyNum() const {
    return mKeyNum;
}

void GameDataFile::addOpenDoorLockNum(s32 count) {
    mOpenDoorLockNum += count;
    mGameDataHolder->setRequireSave();
}

void GameDataFile::setStartedObj(const al::PlacementId* placement_id) {
    if (isStartedObj(placement_id, nullptr))
        return;
    al::StringTmp<128> obj_id;
    for (s32 i = 0; i < 512; i++) {
        if (mStartedObj[i].isEmpty()) {
            placement_id->makeString(&obj_id);
            mStartedObj[i].set(mCurrentStageName, obj_id);
            mGameDataHolder->setRequireSaveFrame();
            break;
        }
    }
}

bool GameDataFile::isStartedObj(const al::PlacementId* placement_id, const char* stage_name) const {
    al::StringTmp<128> obj_id;
    placement_id->makeString(&obj_id);
    al::StringTmp<128> unused;
    if (!stage_name)
        stage_name = mCurrentStageName.cstr();
    for (s32 i = 0; i < 512; i++)
        if (mStartedObj[i].isEqual(stage_name, obj_id.cstr()))
            return true;
    return false;
}

bool GameDataFile::isStartedObj(const char* stage_name, const char* obj_id) const {
    for (s32 i = 0; i < mStartedObj.size(); i++)
        if (mStartedObj[i].isEqual(stage_name, obj_id))
            return true;
    return false;
}

static GameDataFile::SaveObjS32* tryFindSaveObjS32Impl(const al::PlacementId* placement_id,
                                                       const char* stage_name,
                                                       GameDataFile::SaveObjS32* objs) {
    al::StringTmp<128> obj_id;
    placement_id->makeString(&obj_id);
    al::StringTmp<128> unused;
    for (s32 i = 0; i < 256; i++)
        if (objs[i].objInfo.isEqual(stage_name, obj_id.cstr()))
            return &objs[i];
    return nullptr;
}

void GameDataFile::setSaveObjS32(const al::PlacementId* placement_id, s32 value) {
    if (SaveObjS32* obj =
            tryFindSaveObjS32Impl(placement_id, mCurrentStageName.cstr(), mSaveObjS32.begin())) {
        obj->value = value;
        return;
    }
    al::StringTmp<128> obj_id;
    for (s32 i = 0; i < 256; i++) {
        if (mSaveObjS32[i].objInfo.isEmpty()) {
            placement_id->makeString(&obj_id);
            mSaveObjS32[i].objInfo.set(mCurrentStageName, obj_id);
            mSaveObjS32[i].value = value;
            break;
        }
    }
}

const GameDataFile::SaveObjS32*
GameDataFile::tryFindSaveObjS32(const al::PlacementId* placement_id) {
    return tryFindSaveObjS32Impl(placement_id, mCurrentStageName.cstr(), mSaveObjS32.begin());
}

bool GameDataFile::isExistSessionMember(const SessionMusicianType& type) const {
    return mIsExistSessionMember[type];
}

void GameDataFile::addSessionMember(const SessionMusicianType& type) {
    mIsExistSessionMember[type] = true;
}

void GameDataFile::addCoinCollect(const al::PlacementId* placement_id) {
    if (isGotCoinCollect(placement_id))
        return;
    al::StringTmp<128> obj_id;
    placement_id->makeString(&obj_id);
    if (CoinCollectInfo* info = tryFindCoinCollectInfo(mCurrentStageName.cstr(), obj_id.cstr())) {
        info->isGet = true;
        mCoinCollectGotNum[getCurrentWorldIdNoDevelop()]++;
        mGameDataHolder->setRequireSaveFrame();
    }
}

bool GameDataFile::isGotCoinCollect(const al::PlacementId* placement_id) const {
    al::StringTmp<128> obj_id;
    placement_id->makeString(&obj_id);
    const CoinCollectInfo* info = tryFindCoinCollectInfo(mCurrentStageName.cstr(), obj_id.cstr());
    return info && info->isGet;
}

s32 GameDataFile::getCoinCollectNum() const {
    s32 world_id = getCurrentWorldIdNoDevelop();
    return mCoinCollectGotNum[world_id] - mUseCoinCollectNum[world_id];
}

s32 GameDataFile::getCoinCollectGotNum() const {
    return mCoinCollectGotNum[getCurrentWorldIdNoDevelop()];
}

s32 GameDataFile::getCoinCollectGotNum(s32 world_id) const {
    return mCoinCollectGotNum[world_id];
}

void GameDataFile::useCoinCollect(s32 count) {
    mUseCoinCollectNum[getCurrentWorldIdNoDevelop()] += count;
    mGameDataHolder->setRequireSave();
}

const char* GameDataFile::tryFindExistCoinCollectStageName(s32 world_id) const {
    for (s32 i = 0, len = mCoinCollectTable[world_id]->list.size(); i < len; i++) {
        CoinCollectInfo* info = mCoinCollectTable[world_id]->list[i];
        if (checkIsHomeStage(info->stageName.cstr()) || info->isGet)
            continue;
        sead::Vector3f trans = sead::Vector3f::zero;
        if (mGameDataHolder->getWorldList()->tryFindTransOnMainStageByStageName(
                &trans, info->stageName.cstr(), mScenarioNo[world_id]))
            return info->stageName.cstr();
    }

    return nullptr;
}

void GameDataFile::payCoinToSphinx() {
    mIsPayCoinToSphinx = true;
    mGameDataHolder->setRequireSave();
}

bool GameDataFile::isPayCoinToSphinx() const {
    return mIsPayCoinToSphinx;
}

void GameDataFile::answerCorrectSphinxQuiz() {
    mSphinxQuizData->answerCorrectSphinxQuiz(getCurrentWorldIdNoDevelop());
    mGameDataHolder->setRequireSave();
}

void GameDataFile::answerCorrectSphinxQuizAll() {
    mSphinxQuizData->answerCorrectSphinxQuizAll(getCurrentWorldIdNoDevelop());
    mGameDataHolder->setRequireSave();
}

bool GameDataFile::isAnswerCorrectSphinxQuiz(s32 index) const {
    return mSphinxQuizData->isAnswerCorrectSphinxQuiz(index);
}

bool GameDataFile::isAnswerCorrectSphinxQuizAll(s32 index) const {
    return mSphinxQuizData->isAnswerCorrectSphinxQuizAll(index);
}

bool GameDataFile::isTalkAlreadyLocalLanguage() const {
    return mIsTalkLocalLanguage[getCurrentWorldIdNoDevelop()];
}

void GameDataFile::talkLocalLanguage() {
    mIsTalkLocalLanguage[getCurrentWorldIdNoDevelop()] = true;
}

bool GameDataFile::isFirstWorldTravelingStatus() const {
    return al::isEqualString(mWorldTravelingStatus.cstr(), "");
}

void GameDataFile::saveWorldTravelingStatus(const char* status) {
    mWorldTravelingStatus.format(status);
    mGameDataHolder->setRequireSave();
}

const char* GameDataFile::getWorldTravelingStatus() const {
    return mWorldTravelingStatus.cstr();
}

bool GameDataFile::isStartWorldTravelingPeach() const {
    return mIsStartWorldTravelingPeach;
}

void GameDataFile::startWorldTravelingPeach() {
    mIsStartWorldTravelingPeach = true;
}

static GameDataFile::GrowFlower* tryFindGrowFlowerByPot(GameDataFile::GrowFlower* flowers,
                                                        const char* pot_obj) {
    for (s32 i = 0; i < 16; i++)
        if (flowers[i].time != 0 && al::isEqualString(flowers[i].potObj.cstr(), pot_obj))
            return &flowers[i];
    return nullptr;
}

static const GameDataFile::GrowFlower*
tryFindGrowFlowerBySeed(const GameDataFile::GrowFlower* flowers, const char* seed_obj) {
    for (s32 i = 0; i < 16; i++)
        if (flowers[i].time != 0 && al::isEqualString(flowers[i].seedObj.cstr(), seed_obj))
            return &flowers[i];
    return nullptr;
}

static GameDataFile::GrowFlower* tryFindEmptyGrowFlower(GameDataFile::GrowFlower* flowers) {
    for (s32 i = 0; i < 16; i++)
        if (flowers[i].time == 0)
            return &flowers[i];
    return nullptr;
}

void GameDataFile::setGrowFlowerTime(const al::PlacementId* pot_placement_id,
                                     const al::PlacementId* seed_placement_id, u64 time) {
    al::StringTmp<128> pot_obj_id;
    pot_placement_id->makeString(&pot_obj_id);
    al::StringTmp<128> pot_obj("%s_%s", mCurrentStageName.cstr(), pot_obj_id.cstr());
    if (tryFindGrowFlowerByPot(mGrowFlower.begin(), pot_obj.cstr()))
        return;
    al::StringTmp<128> seed_obj_id;
    seed_placement_id->makeString(&seed_obj_id);
    al::StringTmp<128> seed_obj("%s_%s", mCurrentStageName.cstr(), seed_obj_id.cstr());
    if (tryFindGrowFlowerBySeed(mGrowFlower.begin(), seed_obj.cstr()))
        return;
    if (GrowFlower* flower = tryFindEmptyGrowFlower(mGrowFlower.begin())) {
        flower->potObj = pot_obj;
        flower->seedObj = seed_obj;
        flower->time = time;
        mGameDataHolder->setRequireSave();
    }
}

void GameDataFile::setGrowFlowerTime(const al::PlacementId* pot_placement_id, u64 time) {
    al::StringTmp<128> pot_obj_id;
    pot_placement_id->makeString(&pot_obj_id);
    al::StringTmp<128> pot_obj("%s_%s", mCurrentStageName.cstr(), pot_obj_id.cstr());
    if (GrowFlower* flower = tryFindGrowFlowerByPot(mGrowFlower.begin(), pot_obj.cstr())) {
        flower->time = time;
        mGameDataHolder->setRequireSave();
    }
}

u64 GameDataFile::getGrowFlowerTime(const al::PlacementId* pot_placement_id) const {
    al::StringTmp<128> pot_obj_id;
    pot_placement_id->makeString(&pot_obj_id);
    al::StringTmp<128> pot_obj("%s_%s", mCurrentStageName.cstr(), pot_obj_id.cstr());
    const GrowFlower* flower = tryFindGrowFlowerByPot(mGrowFlower.begin(), pot_obj.cstr());
    return flower ? flower->time : 0;
}

void GameDataFile::addGrowFlowerGrowLevel(const al::PlacementId* pot_placement_id, u32 level) {
    al::StringTmp<128> pot_obj_id;
    pot_placement_id->makeString(&pot_obj_id);
    al::StringTmp<128> pot_obj("%s_%s", mCurrentStageName.cstr(), pot_obj_id.cstr());
    if (GrowFlower* flower = tryFindGrowFlowerByPot(mGrowFlower.begin(), pot_obj.cstr()))
        flower->growLevel += level;
}

u32 GameDataFile::getGrowFlowerGrowLevel(const al::PlacementId* pot_placement_id) const {
    al::StringTmp<128> pot_obj_id;
    pot_placement_id->makeString(&pot_obj_id);
    al::StringTmp<128> pot_obj("%s_%s", mCurrentStageName.cstr(), pot_obj_id.cstr());
    const GrowFlower* flower = tryFindGrowFlowerByPot(mGrowFlower.begin(), pot_obj.cstr());
    return flower ? flower->growLevel : 0;
}

bool GameDataFile::isUsedGrowFlowerSeed(const al::PlacementId* seed_placement_id) const {
    al::StringTmp<128> seed_obj_id;
    seed_placement_id->makeString(&seed_obj_id);
    al::StringTmp<128> seed_obj("%s_%s", mCurrentStageName.cstr(), seed_obj_id.cstr());
    return tryFindGrowFlowerBySeed(mGrowFlower.begin(), seed_obj.cstr()) != nullptr;
}

const char* GameDataFile::findGrowFlowerPotIdFromSeedId(const al::PlacementId* seed_placement_id) {
    al::StringTmp<128> seed_obj_id;
    seed_placement_id->makeString(&seed_obj_id);
    al::StringTmp<128> seed_obj("%s_%s", mCurrentStageName.cstr(), seed_obj_id.cstr());
    const GrowFlower* flower = tryFindGrowFlowerBySeed(mGrowFlower.begin(), seed_obj.cstr());
    return al::searchSubString(flower->potObj.cstr(), "_") + 1;
}

void GameDataFile::addCoin(s32 count) {
    mCoinNum = sead::Mathi::min(mCoinNum + count, 9999);
    if (count > 0) {
        clampCount(&mTotalCoinNum, count);
        mTotalCoinNum += count;
    }
}

void GameDataFile::addPlayerJumpCount() {
    mPlayerJumpCount++;
    clampCount(&mPlayerJumpCount);
}

s32 GameDataFile::getPlayerJumpCount() const {
    return mPlayerJumpCount;
}

void GameDataFile::addPlayerThrowCapCount() {
    mPlayerThrowCapCount++;
    clampCount(&mPlayerThrowCapCount);
}

s32 GameDataFile::getPlayerThrowCapCount() const {
    return mPlayerThrowCapCount;
}

bool GameDataFile::readFromStream(sead::ReadStream* stream, u8* buffer) {
    initializeData();
    s32 size = 0;
    stream->readS32(size);
    return size <= 0x200000 && tryReadByamlDataFromStream(stream, buffer, size);
}

bool GameDataFile::tryReadByamlDataFromStream(sead::ReadStream* stream, u8* buffer, s32 size) {
    stream->readMemBlock(buffer, size);
    return tryReadByamlData(buffer);
}

void GameDataFile::writeToStream(sead::WriteStream* stream, sead::Heap* heap) const {
    tryWriteByByaml(stream, heap);
}

bool GameDataFile::isPlayDemoPlayerDownForBattleKoopaAfter() const {
    return mIsPlayDemoPlayerDownForBattleKoopaAfter &&
           al::isEqualString(mCurrentStageName, "MoonWorldBasementStage");
}

s32 GameDataFile::getCheckpointNumMaxInWorld() const {
    return 16;
}

const sead::Vector3f& GameDataFile::getCheckpointTransInWorld(const char* obj_id) const {
    s32 world_id = getCurrentWorldIdNoDevelop();
    for (s32 i = 0; i < 16; i++)
        if (al::isEqualString(mCheckpointTable[world_id][i].objInfo.getObjId(), obj_id))
            return mCheckpointTable[world_id][i].trans;
    return sead::Vector3f::zero;
}

bool GameDataFile::isGotCheckpointInWorld(s32 index) const {
    s32 index2 = calcCheckpointIndexInScenario(index);
    return index2 >= 0 && mCheckpointTable[getCurrentWorldIdNoDevelop()][index2].isGet;
}

const char* GameDataFile::getCheckpointObjIdInWorld(s32 index) const {
    s32 index2 = calcCheckpointIndexInScenario(index);
    if (index2 < 0)
        return "";
    const char* obj_id = mCheckpointTable[getCurrentWorldIdNoDevelop()][index2].objInfo.getObjId();
    if (al::isEqualString(obj_id, ""))
        return "";
    return obj_id;
}

bool GameDataFile::isGotCheckpoint(al::PlacementId* placement_id) const {
    al::StringTmp<128> obj_id;
    obj_id.format("%s", al::makeStringPlacementId(placement_id).cstr());
    for (s32 i = 0; i < mGotCheckpointNum; i++) {
        const UniqObjInfo* info = &mGotCheckpoint[i];
        if (al::isEqualString(info->getStageName(), mCurrentStageName.cstr()) &&
            al::isEqualString(info->getObjId(), obj_id.cstr()))
            return true;
    }
    return false;
}

// TODO why is this function needed?
static void copyString2(const char* src, char* dest, s32 maxlen) {
    al::copyString(dest, src, maxlen);
}

void GameDataFile::changeNextSceneByGotCheckpoint(s32 index) {
    s32 world_id = getCurrentWorldIdNoDevelop();
    const CheckpointInfo& info = mCheckpointTable[world_id][calcCheckpointIndexInScenario(index)];
    mCheckpointWarpObjId.format("%s", info.objInfo.getObjId());
    copyString2(info.objInfo.getObjId(), mCheckpointName.getBuffer(), 128);
    _290.format("%s", info.objInfo.getStageName());
    changeNextSceneByWarp();
}

void GameDataFile::changeNextSceneByWarp() {
    mNextStageName.format("%s", GameDataFunction::tryGetCurrentMainStageName(mGameDataHolder));
    _908.clear();
    mPlayerStartId.clear();
    _160.clear();
    _458 = false;
    mIsWarpCheckpoint = true;
    mGameDataHolder->setStageChanging();
    _a09 = false;
}

void GameDataFile::changeNextSceneByHome() {
    mCheckpointWarpObjId.format("Home");
    mCheckpointName.clear();
    _290.clear();
    changeNextSceneByWarp();
}

void GameDataFile::startYukimaruRace() {
    mRaceType = RaceType_Yukimaru;
}

void GameDataFile::startYukimaruRaceTutorial() {
    mRaceType = RaceType_YukimaruTutorial;
}

void GameDataFile::startRaceManRace() {
    mRaceType = RaceType_Flag;
}

void GameDataFile::registerCheckpointTrans(const al::PlacementId* placement_id,
                                           const sead::Vector3f& trans) {
    if (CheckpointInfo* info =
            tryFindCheckpointInfoImpl(mCheckpointTable, mCurrentStageName.cstr(),
                                      al::makeStringPlacementId(placement_id).cstr()))
        info->trans.set(trans);
}

s32 GameDataFile::calcGetCheckpointNum() const {
    s32 count = 0;
    for (s32 i = 0; i < mGotCheckpoint.size(); i++)
        if (!mGotCheckpoint[i].isEmpty())
            count++;
    return count;
}

bool GameDataFile::isEnableUnlockHint() const {
    for (s32 i = 0; i < 1024; i++)
        if (mHintList[i].isEnableUnlock(mCurrentWorldId, false,
                                        mScenarioNo[getCurrentWorldIdNoDevelop()], isGameClear()))
            return true;
    return false;
}

s32 GameDataFile::calcRestHintNum() const {
    s32 count = 0;
    for (s32 i = 0; i < 1024; i++)
        if (mHintList[i].isEnableUnlock(mCurrentWorldId, false,
                                        mScenarioNo[getCurrentWorldIdNoDevelop()], isGameClear()))
            count++;
    return count;
}

void GameDataFile::unlockHint() {
    unlockHintImpl(HintStatus_UnlockByNpc);
}

void GameDataFile::unlockHintImpl(s32 hint_status) {
    const sead::PtrArray<HintInfo>& list = mHintTableByIdx[getCurrentWorldIdNoDevelop()]->list;
    for (s32 i = 0, len = list.size(); i < len; i++) {
        if (list[i]->isEnableUnlock(mCurrentWorldId, false,
                                    mScenarioNo[getCurrentWorldIdNoDevelop()], isGameClear())) {
            list[i]->hintStatus = hint_status;
            mGameDataHolder->setRequireSave();
            mLatestHint = list[i];
            return;
        }
    }
}

void GameDataFile::unlockHintAmiibo() {
    unlockHintImpl(HintStatus_UnlockByAmiibo);
}

void GameDataFile::unlockHintAddByMoonRock() {
    bool success = false;
    const sead::PtrArray<HintInfo>& list = mHintTable[getCurrentWorldIdNoDevelop()]->list;
    for (s32 i = 0, len = list.size(); i < len; i++) {
        HintInfo* info = list[i];
        if (info->isEnableUnlock(mCurrentWorldId, true, mScenarioNo[getCurrentWorldIdNoDevelop()],
                                 isGameClear())) {
            info->hintStatus = HintStatus_UnlockByNpc;
            success = true;
        }
    }
    if (success)
        mGameDataHolder->setRequireSave();
}

s32 GameDataFile::calcHintNum() const {
    s32 count = 0;
    for (s32 i = 0; i < 1024; i++)
        if (mHintList[i].isHintStatusUnlock(mCurrentWorldId, mScenarioNoPlacement, false))
            count++;
    return count;
}

const sead::Vector3f& GameDataFile::calcHintTrans(s32 index) const {
    return findHint(index).trans;
}

const GameDataFile::HintInfo& GameDataFile::findHint(s32 index) const {
    s32 count = 0;
    for (s32 i = 0; i < 1024; i++) {
        if (mHintList[i].isHintStatusUnlock(mCurrentWorldId, mScenarioNoPlacement, false)) {
            if (count == index)
                return mHintList[i];
            count++;
        }
    }
    return mHintList[0];
}

const sead::Vector3f& GameDataFile::calcHintTransMostEasy() const {
    return findHintInfoMostEasy()->trans;
}

const GameDataFile::HintInfo* GameDataFile::findHintInfoMostEasy() const {
    s32 world_id = getCurrentWorldIdNoDevelop();
    const sead::PtrArray<HintInfo>& list = mHintTableByIdx[world_id]->list;
    for (s32 i = 0, len = list.size(); i < len; i++)
        if (list[i]->isHintStatusUnlock(world_id, mScenarioNoPlacement, false))
            return list[i];
    return nullptr;
}

s32 GameDataFile::calcHintMoonRockNum() const {
    s32 count = 0;
    for (s32 i = 0; i < 1024; i++)
        if (mHintList[i].isHintStatusUnlock(mCurrentWorldId, mScenarioNoPlacement, true))
            count++;
    return count;
}

const sead::Vector3f& GameDataFile::calcHintMoonRockTrans(s32 index) const {
    return findHintMoonRock(index).trans;
}

const GameDataFile::HintInfo& GameDataFile::findHintMoonRock(s32 index) const {
    s32 count = 0;
    for (s32 i = 0; i < 1024; i++) {
        if (mHintList[i].isHintStatusUnlock(mCurrentWorldId, mScenarioNoPlacement, true)) {
            if (count == index)
                return mHintList[i];
            count++;
        }
    }
    return mHintList[0];
}

bool GameDataFile::tryUnlockShineName(s32 world_id, s32 index) {
    s32 unlock_count = 0;
    s32 count = 0;
    s32 index2 = 0;
    for (s32 i = 0; i < 1024; i++) {
        const HintInfo* info = &mHintList[i];
        if (info->worldId == world_id && (!info->isMoonRock || isOpenMoonRock(world_id))) {
            if (info->status == AchievementStatus_Unlocked && !info->isAchievement)
                unlock_count++;
            if (unlock_count > 2)
                return false;
            if (count == index)
                index2 = i;
            count++;
        }
    }
    mHintList[index2].status = AchievementStatus_Unlocked;
    return true;
}

bool GameDataFile::isOpenMoonRock(s32 world_id) const {
    return mGameDataHolder->getWorldList()->getMoonRockScenarioNo(sead::Mathi::max(world_id, 0)) ==
           mScenarioNo[world_id];
}

void GameDataFile::calcShineIndexTableNameAvailable(s32* index_table, s32* index_table_len,
                                                    s32 world_id) {
    s32 index = 0;
    s32 len = 0;
    for (s32 i = 0; i < 1024; i++) {
        const HintInfo* info = &mHintList[i];
        if (info->worldId == world_id && (!info->isMoonRock || isOpenMoonRock(world_id))) {
            if (info->status == AchievementStatus_Unlocked) {
                index_table[len] = index;
                len++;
            }
            index++;
        }
    }
    *index_table_len = len;
}

void GameDataFile::calcShineIndexTableNameUnlockable(s32* index_table, s32* index_table_len,
                                                     s32 world_id) {
    world_id = sead::Mathi::max(world_id, 0);
    s32 index = 0;
    s32 len = 0;
    for (s32 i = 0; i < 1024; i++) {
        const HintInfo* info = &mHintList[i];
        if (info->worldId == world_id && (!info->isMoonRock || isOpenMoonRock(world_id))) {
            if (info->status == AchievementStatus_None && !info->isAchievement &&
                info->mainScenarioNo == -1 &&
                info->isEnableNameUnlockByScenario(world_id, mScenarioNo[world_id],
                                                   isGameClear())) {
                index_table[len] = index;
                len++;
            }
            index++;
        }
    }
    *index_table_len = len;
}

bool GameDataFile::isUnlockAchievementShineName() const {
    return mIsUnlockAchievement;
}

void GameDataFile::unlockWorld(s32 world_id) {
    mGameProgressData->unlockNextWorld(world_id);
    if (mUnlockedWorldId[world_id] >= 0)
        return;
    mUnlockedWorldId[world_id] = mUnlockedWorldNum;
    mUnlockedWorldNum++;
    if (world_id == GameDataFunction::getWorldIndexClash() ||
        world_id == GameDataFunction::getWorldIndexBoss()) {
        s32 _w8 = 0;
        for (s32 i = 0; i < 20; i++)
            if (mUnlockedWorldId[i] >= mUnlockedWorldId[_w8] && i != world_id)
                _w8 = i;
        if (_w8 > world_id) {
            mUnlockedWorldId[world_id] = world_id;
            mUnlockedWorldId[_w8] = mUnlockedWorldNum - 1;
        }
    }
    mWorldMapNum = 0;
    updateWorldMapIndex();
    updateWorldWarpIndex();
    if (world_id == GameDataFunction::getWorldIndexPeach())
        setGameClear();
    mGameDataHolder->setRequireSave();
}

void GameDataFile::noPlayDemoWorldWarp() {
    if (_9f8 != 0)
        mIsPlayAlreadyWorldWarp = true;
    _9f8 = 0;
}

s32 GameDataFile::calcWorldWarpHoleThroughNum() const {
    s32 count = 0;
    for (s32 i = 0; i < mIsWorldWarpHoleThrough.size(); i++)
        if (mIsWorldWarpHoleThrough[i])
            count++;
    return count;
}

s32 GameDataFile::getWorldWarpHoleThroughNumMax() const {
    return mGameDataHolder->getWorldList()->getWorldNum() - 7;
}

void GameDataFile::enteredStage() {
    mIsEnterStageFirst = false;
    _459 = false;
    _a09 = true;
    mIsPlayDemoReturnToHome = false;
    mIsPlayDemoAwardSpecial = false;
    mIsWarpCheckpoint = false;
    mIsMissEndPrevStage = false;
    mIsPlayDemoWorldWarpHole = false;
    mRaceResult = RaceResult_None;
    mLastRaceRanking = 0;
}

void GameDataFile::buyDefaultItem() {
    const sead::PtrArray<ShopItem::ItemInfo>& clothes = mGameDataHolder->getClothList();
    for (s32 i = 0; i < clothes.size(); i++) {
        if (al::isEqualString(clothes[i]->name, "Mario")) {
            buyItem(clothes[i], false);
            break;
        }
    }
    const sead::PtrArray<ShopItem::ItemInfo>& caps = mGameDataHolder->getCapList();
    for (s32 i = 0; i < caps.size(); i++) {
        if (al::isEqualString(caps[i]->name, "Mario")) {
            buyItem(caps[i], false);
            break;
        }
    }
}

void GameDataFile::buyItem(const ShopItem::ItemInfo* info, bool is_save_prepo) {
    sead::FixedSafeString<64>* item_list = tryFindItemList(info);
    if (!item_list)
        return;
    for (s32 i = 0; i < 64; i++) {
        if (al::isEqualString(item_list[i].cstr(), info->name))
            return;
        if (item_list[i].isEmpty()) {
            item_list[i].format(info->name);
            break;
        }
    }
    mGameDataHolder->setRequireSaveFrame();
    if (is_save_prepo) {
        switch (info->type) {
        case ShopItem::ItemType::Cloth:
            rs::trySavePrepoGetClothEvent(info->name, calcHaveClothNum(), mPlayTimeTotal,
                                          mSaveDataIdForPrepo,
                                          mGameDataHolder->getPlayTimeAcrossFile());
            break;
        case ShopItem::ItemType::Cap:
            rs::trySavePrepoGetCapEvent(info->name, calcHaveCapNum(), mPlayTimeTotal,
                                        mSaveDataIdForPrepo,
                                        mGameDataHolder->getPlayTimeAcrossFile());
            break;
        case ShopItem::ItemType::Sticker:
            rs::trySavePrepoGetStickerEvent(info->name, calcHaveStickerNum(), mPlayTimeTotal,
                                            mSaveDataIdForPrepo,
                                            mGameDataHolder->getPlayTimeAcrossFile());
            break;
        case ShopItem::ItemType::Gift:
            rs::trySavePrepoGetGiftEvent(info->name, calcHaveGiftNum(), mPlayTimeTotal,
                                         mSaveDataIdForPrepo,
                                         mGameDataHolder->getPlayTimeAcrossFile());
            break;
        case ShopItem::ItemType::UseItem:
        case ShopItem::ItemType::Moon:
            break;
        }
    }
}

sead::FixedSafeString<64>* GameDataFile::tryFindItemList(const ShopItem::ItemInfo* info) const {
    switch (info->type) {
    case ShopItem::ItemType::Cloth:
        return mItemCloth.begin();
    case ShopItem::ItemType::Cap:
        return mItemCap.begin();
    case ShopItem::ItemType::Sticker:
        return mItemSticker.begin();
    case ShopItem::ItemType::Gift:
        return mItemGift.begin();
    default:
        return nullptr;
    }
}

bool GameDataFile::isBuyItem(const ShopItem::ItemInfo* info) const {
    const sead::FixedSafeString<64>* item_list = tryFindItemList(info);
    return item_list && isBuyItem(info->name, item_list);
}

bool GameDataFile::isBuyItem(const char* name, const sead::FixedSafeString<64>* item_list) const {
    for (s32 i = 0; i < 64; i++) {
        if (item_list[i].isEmpty())
            break;
        if (al::isEqualString(item_list[i].cstr(), name))
            return true;
    }
    return false;
}

s32 GameDataFile::calcHaveClothNum() const {
    const sead::PtrArray<ShopItem::ItemInfo>& items = mGameDataHolder->getClothList();
    s32 count = 0;
    for (s32 i = 0; i < items.size(); i++)
        if (isBuyItem(items[i]->name, mItemCloth.begin()))
            count++;
    return count;
}

s32 GameDataFile::calcHaveCapNum() const {
    const sead::PtrArray<ShopItem::ItemInfo>& items = mGameDataHolder->getCapList();
    s32 count = 0;
    for (s32 i = 0; i < items.size(); i++)
        if (isBuyItem(items[i]->name, mItemCap.begin()))
            count++;
    return count;
}

s32 GameDataFile::calcHaveStickerNum() const {
    const sead::PtrArray<ShopItem::ItemInfo>& items = mGameDataHolder->getStickerList();
    s32 count = 0;
    for (s32 i = 0; i < items.size(); i++)
        if (isBuyItem(items[i]->name, mItemSticker.begin()))
            count++;
    return count;
}

s32 GameDataFile::calcHaveGiftNum() const {
    const sead::PtrArray<ShopItem::ItemInfo>& items = mGameDataHolder->getGiftList();
    s32 count = 0;
    for (s32 i = 0; i < items.size(); i++)
        if (isBuyItem(items[i]->name, mItemGift.begin()))
            count++;
    return count;
}

void GameDataFile::buyItemAll() {
    const sead::PtrArray<ShopItem::ShopItemInfo>& shop_items =
        mGameDataHolder->getShopItemInfoList();
    for (s32 i = 0, len = shop_items.size(); i < len; i++)
        buyItem(&shop_items[i]->info, false);

    const sead::PtrArray<ShopItem::ItemInfo>& clothes = mGameDataHolder->getClothList();
    for (s32 i = 0; i < clothes.size(); i++)
        buyItem(clothes[i], false);

    const sead::PtrArray<ShopItem::ItemInfo>& caps = mGameDataHolder->getCapList();
    for (s32 i = 0; i < caps.size(); i++)
        buyItem(caps[i], false);

    const sead::PtrArray<ShopItem::ItemInfo>& stickers = mGameDataHolder->getStickerList();
    for (s32 i = 0; i < stickers.size(); i++)
        buyItem(stickers.unsafeAt(i), false);

    const sead::PtrArray<ShopItem::ItemInfo>& gifts = mGameDataHolder->getGiftList();
    for (s32 i = 0; i < gifts.size(); i++)
        buyItem(gifts[i], false);
}

void GameDataFile::wearCostume(const char* name) {
    mCurrentCostumeName.format("%s", name);
}

void GameDataFile::wearCap(const char* name) {
    mCurrentCapName.format("%s", name);
}

void GameDataFile::addHackDictionary(const char* hack_name) {
    for (s32 i = 0; i < 160; i++) {
        if (mHackDictionary[i].isEmpty()) {
            mHackDictionary[i].format("%s", hack_name);
            rs::trySavePrepoFirstHackEvent(
                hack_name, rs::calcHackObjNum(mGameDataHolder), mPlayTimeTotal, mSaveDataIdForPrepo,
                GameDataFunction::getPlayTimeAcrossFile(mGameDataHolder));
            break;
        }
        if (al::isEqualString(mHackDictionary[i].cstr(), hack_name))
            break;
    }
}

bool GameDataFile::isExistInHackDictionary(const char* hack_name) const {
    for (s32 i = 0; i < 160; i++) {
        if (al::isEqualString(mHackDictionary[i].cstr(), hack_name))
            return true;
        if (mHackDictionary[i].isEmpty())
            break;
    }
    return false;
}

const GameDataFile::HintInfo* GameDataFile::findShine(s32 world_id, s32 index) const {
    s32 count = 0;
    for (s32 i = 0; i < 1024; i++) {
        if (mHintList[i].worldId == world_id &&
            (!mHintList[i].isMoonRock || isOpenMoonRock(world_id))) {
            if (count == index)
                return &mHintList[i];
            count++;
        }
    }
    return nullptr;
}

bool GameDataFile::isGotShine(s32 world_id, s32 index) const {
    return findShine(world_id, index)->isGet;
}

bool GameDataFile::isOpenShineName(s32 world_id, s32 index) const {
    return findShine(world_id, index)->status == AchievementStatus_Unlocked;
}

s32 GameDataFile::calcShineNumInOneShine(s32 world_id, s32 index) const {
    const HintInfo* info = findShine(world_id, index);
    return info->isGrand ? 3 : info->isShop ? mShopShineNum[world_id] : 1;
}

bool GameDataFile::checkAchievementShine(s32 world_id, s32 index) const {
    return findShine(world_id, index)->isAchievement;
}

s32 GameDataFile::getWorldTotalShineNum(s32 world_id) const {
    return world_id == GameDataFunction::getWorldIndexPeach() && mIsUnlockAchievement ?
               isOpenMoonRock(world_id) ?
               mTotalShineNum[world_id] + mTotalMoonRockShineNum[world_id] + mTotalAchievementNum :
               mTotalShineNum[world_id] + mTotalAchievementNum :
           isOpenMoonRock(world_id) ? mTotalShineNum[world_id] + mTotalMoonRockShineNum[world_id] :
                                      mTotalShineNum[world_id];
}

s32 GameDataFile::getWorldTotalShineNumMax(s32 world_id) const {
    return world_id == GameDataFunction::getWorldIndexPeach() ?
               mTotalShineNum[world_id] + mTotalMoonRockShineNum[world_id] + mTotalAchievementNum :
               mTotalShineNum[world_id] + mTotalMoonRockShineNum[world_id];
}

void GameDataFile::winRace() {
    mRaceResult = RaceResult_Win;
}

RaceRecord* GameDataFile::findRaceRecord(const char* name) const {
    for (s32 i = 0; i < mRaceRecord.size(); i++)
        if (mRaceRecord[i]->isEqualName(name))
            return mRaceRecord[i];
    return nullptr;
}

void GameDataFile::incrementRaceLoseCount(s32 level) {
    switch (level) {
    case 0:
        mRaceLoseCountLv1++;
        break;
    case 1:
        mRaceLoseCountLv2++;
        break;
    }
}

s32 GameDataFile::getRaceLoseCount(s32 level) const {
    switch (level) {
    case 0:
        return mRaceLoseCountLv1;
    case 1:
        return mRaceLoseCountLv2;
    default:
        return 0;
    }
}

void GameDataFile::setJumpingRopeBestCount(s32 count) {
    mIsExistRecordJumpingRope = true;
    mJumpingRopeBestCount = count;
}

void GameDataFile::setUpdateJumpingRopeScoreFlag() {
    mNetworkUploadFlag->updateScore(RankingCategory::JumpingRope);
}

void GameDataFile::setVolleyballBestCount(s32 count) {
    mIsExistRecordVolleyball = true;
    mVolleyballBestCount = count;
}

void GameDataFile::setUpdateVolleyballScoreFlag() {
    mNetworkUploadFlag->updateScore(RankingCategory::Volleyball);
}

bool GameDataFile::isExistJango() const {
    return mIsEnableJangoTrans || mCapStatusForJango == CapStatus::Removed;
}

void GameDataFile::setJangoTrans(const sead::Vector3f& trans) {
    mJangoTrans.set(trans);
    mIsEnableJangoTrans = true;
}

void GameDataFile::setAmiiboNpcTrans(const sead::Vector3f& trans) {
    mAmiiboNpcTrans.set(trans);
    mIsExistAmiiboNpc = true;
}

void GameDataFile::setTimeBalloonNpcTrans(const sead::Vector3f& trans) {
    mTimeBalloonNpcTrans.set(trans);
    mIsExistTimeBalloonNpc = true;
}

void GameDataFile::setPoetterTrans(const sead::Vector3f& trans) {
    mPoetterTrans.set(trans);
    mIsExistPoetter = true;
}

void GameDataFile::setShopNpcTrans(const sead::Vector3f& trans, const char* name, s32 type) {
    for (s32 i = 0; i < 4; i++) {
        if (!mShopNpcInfo[i].name.isEmpty() &&
            al::isEqualString(name, mShopNpcInfo[i].name.cstr()) && type == mShopNpcInfo[i].type) {
            mShopNpcInfo[i].trans.set(trans);
            break;
        }
        if (mShopNpcInfo[i].name.isEmpty()) {
            mShopNpcInfo[i].trans.set(trans);
            mShopNpcInfo[i].name.format("%s", name);
            mShopNpcInfo[i].type = type;
            break;
        }
    }
}

void GameDataFile::setMoonRockTrans(const sead::Vector3f& trans) {
    mMoonRockTrans.set(trans);
    mIsExistMoonRock = true;
}

void GameDataFile::setMiniGameInfo(const sead::Vector3f& trans, const char* name) {
    for (s32 i = 0; i < 4; i++) {
        if (mMiniGameInfo[i].isExist && al::isEqualString(mMiniGameInfo[i].name.cstr(), name)) {
            mMiniGameInfo[i].trans = trans;
            break;
        }
        if (!mMiniGameInfo[i].isExist) {
            mMiniGameInfo[i].trans = trans;
            mMiniGameInfo[i].isExist = true;
            mMiniGameInfo[i].name.format("%s", name);
            break;
        }
    }
}

s32 GameDataFile::calcMiniGameNum() const {
    s32 count = 0;
    for (s32 i = 0; i < 4; i++)
        if (mMiniGameInfo[i].isExist)
            count++;
    return count;
}

s32 GameDataFile::getMiniGameNumMax() const {
    return 4;
}

const sead::Vector3f& GameDataFile::getMiniGameTrans(s32 index) const {
    return mMiniGameInfo[index].trans;
}

const char* GameDataFile::getMiniGameName(s32 index) const {
    return mMiniGameInfo[index].name.cstr();
}

bool GameDataFile::isExistTimeBalloonNpc() const {
    return mIsExistTimeBalloonNpc;
}

const sead::Vector3f& GameDataFile::getTimeBalloonNpcTrans() const {
    return mTimeBalloonNpcTrans;
}

bool GameDataFile::isExistPoetter() const {
    return mIsExistPoetter;
}

const sead::Vector3f& GameDataFile::getPoetterTrans() const {
    return mPoetterTrans;
}

bool GameDataFile::isAlreadyShowExplainCheckpointFlag() const {
    return mIsShowExplainCheckpointFlag;
}

void GameDataFile::showExplainCheckpointFlag() {
    mIsShowExplainCheckpointFlag = true;
    mGameDataHolder->setRequireSave();
}

const sead::Vector3f& GameDataFile::getShopNpcTrans(s32 index) const {
    return mShopNpcInfo[index].trans;
}

bool GameDataFile::isShopSellout(s32 index) const {
    const char* name = mShopNpcInfo[index].name.cstr();
    const sead::PtrArray<ShopItem::ShopItemInfo>& items = mGameDataHolder->getShopItemInfoList();
    s32 type = mShopNpcInfo[index].type;
    for (s32 i = 0; i < items.size(); i++) {
        switch (type) {
        case 0:
            if (*items[i]->storeName != 0 && !al::isEqualString(items[i]->storeName, name))
                continue;
            break;
        case 1:
            if (*items[i]->storeName != 0)
                continue;
            break;
        case 2:
            if (!al::isEqualString(items[i]->storeName, name))
                continue;
            break;
        }
        if (!isBuyItem(&items[i]->info))
            return false;
    }
    return true;
}

s32 GameDataFile::calcShopNum() const {
    s32 count = 0;
    for (s32 i = 0; i < 4; i++) {
        if (mShopNpcInfo[i].name.isEmpty())
            break;
        count++;
    }
    return count;
}

s32 GameDataFile::getShopNpcIconNumMax() const {
    return 4;
}

s32 GameDataFile::getScenarioNo() const {
    if (mScenarioNoOverride != -1)
        return mScenarioNoOverride;
    if (checkIsHomeStage(mCurrentStageName.cstr()))
        return mScenarioNoPlacement;
    return mScenarioNo[getCurrentWorldIdNoDevelop()];
}

s32 GameDataFile::getScenarioNoPlacement() const {
    return mScenarioNoPlacement;
}

bool GameDataFile::isClearWorldMainScenario(s32 world_id) const {
    const WorldList* world_list = mGameDataHolder->getWorldList();
    return world_list->isEqualClearMainScenarioNo(world_id, mScenarioNo[world_id]) ||
           world_list->isEqualAfterEndingScenarioNo(world_id, mScenarioNo[world_id]) ||
           world_list->isEqualMoonRockScenarioNo(world_id, mScenarioNo[world_id]);
}

s32 GameDataFile::calcCheckpointIndexInScenario(s32 index) const {
    s32 count = 0;
    for (s32 i = 0; i <= 16; i++) {
        if (mCheckpointTable[getCurrentWorldIdNoDevelop()][i].scenarios.isOnBit(getScenarioNo())) {
            if (count == index)
                return i;
            count++;
        }
    }
    return -1;
}

bool GameDataFile::isFlagOnTalkMessageInfo(s32 index) const {
    return mFlagTalkMessage[index];
}

void GameDataFile::setFlagOnTalkMessageInfo(s32 index) {
    mFlagTalkMessage[index] = true;
}

bool GameDataFile::isTalkKakku() const {
    return mIsTalkKakku;
}

void GameDataFile::talkKakku() {
    mIsTalkKakku = true;
}

bool GameDataFile::isTalkWorldTravelingPeach() const {
    return mIsTalkWorldTravelingPeach;
}

void GameDataFile::talkWorldTravelingPeach() {
    mIsTalkWorldTravelingPeach = true;
}

bool GameDataFile::isTalkCollectBgmNpc() const {
    return mIsTalkCollectBgmNpc;
}

void GameDataFile::talkCollectBgmNpc() {
    mIsTalkCollectBgmNpc = true;
}

s32 GameDataFile::getTokimekiMayorNpcFavorabilityRating() const {
    return mTokimekiMayorNpcFavorabilityRating;
}

void GameDataFile::setTokimekiMayorNpcFavorabilityRating(s32 rating) {
    mTokimekiMayorNpcFavorabilityRating = rating;
}

bool GameDataFile::isFirstNetwork() const {
    return mIsFirstNetwork;
}

void GameDataFile::noFirstNetwork() {
    mIsFirstNetwork = false;
    mGameDataHolder->setRequireSaveFrame();
}

void GameDataFile::setKidsMode(bool is_kids_mode) {
    if (mIsKidsMode == is_kids_mode)
        return;
    mIsKidsMode = is_kids_mode;
    mPlayerHitPointData->setKidsModeFlag(is_kids_mode);
    mPlayerHitPointData->recoverMax();
}

bool GameDataFile::isPlayScenarioCamera(const QuestInfo* info) const {
    return !mGameDataHolder->isPlayAlreadyScenarioStartCamera(info->getQuestNo()) &&
           isNextMainShine(info);
}

bool GameDataFile::isNextMainShine(const QuestInfo* info) const {
    if (info && rs::getActiveQuestNum(info) > 0 && info->isMainQuest()) {
        if (rs::isActiveQuest(info))
            return true;
        if (rs::getActiveQuestList(info)[0]->getQuestNo() == info->getQuestNo())
            return true;
    }
    return false;
}

bool GameDataFile::isNextMainShine(s32 index) const {
    return mHintList[index].mainScenarioNo == mMainScenarioNo[getCurrentWorldIdNoDevelop()] + 1;
}

bool GameDataFile::isMainShine(s32 index) const {
    if (index < 0)
        return false;
    return mHintList[index].mainScenarioNo >= 0;
}

bool GameDataFile::isLatestGetMainShine(const ShineInfo* info) const {
    for (s32 i = 0; i < 1024; i++)
        if (isEqual(&mHintList[i], info) &&
            mHintList[i].mainScenarioNo == mMainScenarioNo[getCurrentWorldIdNoDevelop()] &&
            mCurrentWorldId == mHintList[i].worldId)
            return true;
    return false;
}

bool GameDataFile::calcIsGetMainShineAll(const al::IUseSceneObjHolder* scene_obj_holder) const {
    return !tryFindNextMainScenarioInfo() && rs::getActiveQuestNum(scene_obj_holder) == 0;
}

bool GameDataFile::calcIsGetShineAllInWorld(s32 world_id) const {
    for (s32 i = 0; i < 1024; i++)
        if (mHintList[i].worldId == world_id && !mHintList[i].isGet)
            return false;
    if (world_id == GameDataFunction::getWorldIndexPeach()) {
        if (mAchievementSaveData->calcGetAchievementNum() !=
            mAchievementSaveData->getAchievementNum())
            return false;
    }
    return true;
}

s32 GameDataFile::tryFindLinkedShineIndex(const al::ActorInitInfo& actor_info,
                                          const al::IUseSceneObjHolder* scene_obj_holder) const {
    return tryFindLinkedShineIndex(actor_info, 0, scene_obj_holder);
}

s32 GameDataFile::tryFindLinkedShineIndex(const al::ActorInitInfo& actor_info, s32 link_index,
                                          const al::IUseSceneObjHolder* scene_obj_holder) const {
    ShineInfo shine_info;
    const char* stage_name = nullptr;
    const char* obj_id = nullptr;
    al::PlacementInfo placement_info;
    bool has_shine_actor = al::isExistLinkChild(actor_info, "ShineActor", 0);
    if (al::isExistLinkChild(actor_info, "ShineDummy", 0)) {
        al::getLinksInfoByIndex(&placement_info, actor_info, "ShineDummy", link_index);
        al::getStringArg(&stage_name, placement_info, "StageName");
        al::getStringArg(&obj_id, placement_info, "ObjId");
    } else {
        if (!al::isExistLinkChild(actor_info, "NoDelete_Shine", 0) && !has_shine_actor)
            return -1;
        const char* link_name = has_shine_actor ? "ShineActor" : "NoDelete_Shine";
        al::PlacementId placement_id;
        al::getLinksInfoByIndex(&placement_info, actor_info, link_name, link_index);
        al::getPlacementId(&placement_id, placement_info);
        obj_id = placement_id.getId();
        stage_name = rs::getPlacementStageName(scene_obj_holder, placement_info);
    }
    shine_info.init(stage_name, obj_id, "", nullptr, ShineInfo::Type_Shine);
    s32 index = tryFindShineIndexImpl(mHintList.begin(), &shine_info);
    return index < 0 ? -1 : index;
}

s32 GameDataFile::tryFindLinkedShineIndexByLinkName(const al::IUseSceneObjHolder* scene_obj_holder,
                                                    const al::ActorInitInfo& actor_info,
                                                    const char* link_name) const {
    ShineInfo shine_info;
    al::PlacementInfo placement_info;
    if (!al::isExistLinkChild(actor_info, link_name, 0))
        return -1;
    al::PlacementId placement_id;
    al::getLinksInfoByIndex(&placement_info, actor_info, link_name, 0);
    al::getPlacementId(&placement_id, placement_info);
    al::StringTmp<128> obj_id;
    obj_id.format("%s", placement_id.getId());
    const char* stage_name = rs::getPlacementStageName(scene_obj_holder, placement_info);
    shine_info.init(stage_name, obj_id.cstr(), "", nullptr, ShineInfo::Type_Shine);
    s32 index = tryFindShineIndexImpl(mHintList.begin(), &shine_info);
    return index < 0 ? -1 : index;
}

s32 GameDataFile::calcLinkedShineNum(const al::ActorInitInfo& actor_info) const {
    ShineInfo shine_info;
    al::PlacementInfo placement_info;
    if (al::isExistLinkChild(actor_info, "ShineDummy", 0))
        return al::calcLinkChildNum(actor_info, "ShineDummy");
    if (al::isExistLinkChild(actor_info, "NoDelete_Shine", 0))
        return al::calcLinkChildNum(actor_info, "NoDelete_Shine");
    return -1;
}

s32 GameDataFile::tryFindShineIndex(const al::ActorInitInfo& actor_info) const {
    al::PlacementId placement_id;
    al::getPlacementId(&placement_id, actor_info);
    const char* obj_id = placement_id.getId();
    const char* stage_name = rs::getPlacementStageName(mGameDataHolder, actor_info);
    ShineInfo shine_info;
    shine_info.init(stage_name, obj_id, "", nullptr, ShineInfo::Type_Shine);
    s32 index = tryFindShineIndexImpl(mHintList.begin(), &shine_info);
    return index < 0 ? -1 : index;
}

s32 GameDataFile::tryFindShineIndex(const char* stage_name, const char* obj_id) const {
    ShineInfo shine_info;
    shine_info.init(stage_name, obj_id, "", nullptr, ShineInfo::Type_Shine);
    s32 index = tryFindShineIndexImpl(mHintList.begin(), &shine_info);
    return index < 0 ? -1 : index;
}

void GameDataFile::disableHintById(s32 index) {
    if (index < 0)
        return;
    mHintList[index].isDisabled = true;
}

void GameDataFile::enableHintById(s32 index) {
    if (index < 0)
        return;
    mHintList[index].isDisabled = false;
}

void GameDataFile::setStartShine(const ShineInfo* info) {
    mStartShineIndex = tryFindShineIndexImpl(mHintList.begin(), info);
}

s32 GameDataFile::getStartShineNextIndex() const {
    if (mStartShineIndex < 0)
        return -1;
    s32 next_main_scenario_no = mHintList[mStartShineIndex].mainScenarioNo + 1;
    s32 world_id = mHintList[mStartShineIndex].worldId;
    for (s32 i = 0; i < 1024; i++)
        if (mHintList[i].mainScenarioNo == next_main_scenario_no &&
            world_id == mHintList[i].worldId)
            return i;
    return -1;
}

void GameDataFile::setHintTrans(s32 index, const sead::Vector3f& trans) {
    mHintList[index].trans.set(trans);
}

void GameDataFile::setOriginalHintTrans(s32 index) {
    resetHintTrans(index);
}

void GameDataFile::resetHintTrans(s32 index) {
    HintInfo* info = &mHintList[index];
    if (!mGameDataHolder->getWorldList()->tryFindHintTransByScenarioNo(
            &info->trans, info->uniqueId, mScenarioNo[sead::Mathi::max(info->worldId, 0)]))
        info->trans.set(info->originalTrans);
}

void GameDataFile::registerShineInfo(const ShineInfo* info, const sead::Vector3f& trans) {
    for (s32 i = 0; i < 1024; i++) {
        if (isEqual(&mHintList[i], info)) {
            if (const QuestInfo* quest_info = info->getQuestInfo())
                mHintList[i].mainScenarioNo = quest_info->getQuestNo();
            break;
        }
        if (mHintList[i].stageName.isEmpty()) {
            mHintList[i].stageName.format("%s", info->getStageName());
            mHintList[i].objId.format("%s", info->getObjId());
            mHintList[i].trans.set(trans);
            if (const QuestInfo* quest_info = info->getQuestInfo())
                mHintList[i].mainScenarioNo = quest_info->getQuestNo();
            else
                mHintList[i].mainScenarioNo = -1;
            mHintList[i].worldId = mCurrentWorldId;
            if (mCurrentWorldId != -1) {
                mHintTable[mCurrentWorldId]->list.pushBack(&mHintList[i]);
                mHintTableByIdx[mCurrentWorldId]->list.pushBack(&mHintList[i]);
            }
            break;
        }
    }
}

s32 GameDataFile::calcRestShineInStageWithWorldProgress(const char* stage_name) const {
    s32 world_id = mGameDataHolder->getWorldList()->tryFindWorldIndexByStageName(stage_name);
    s32 scenario_no = mScenarioNo[sead::Mathi::max(world_id, 0)];
    s32 count = 0;
    for (s32 i = 0; i < 1024; i++)
        if (!mHintList[i].isGet && al::isEqualString(mHintList[i].stageName.cstr(), stage_name) &&
            mHintList[i].progressBitFlag.isOnBit(scenario_no - 1))
            count++;
    return count;
}

s32 GameDataFile::calcGetShineNumByObjectNameOrOptionalId(const char* object_name_or_optional_id,
                                                          CountType count_type) const {
    s32 count = 0;
    bool has_shine[sNumWorlds] = {};
    for (s32 i = 0; i < 1024; i++) {
        if (count_type == CountType::Value_0 && has_shine[mHintList[i].worldId])
            continue;
        bool matches =
            count_type == CountType::Value_2 ?
                al::isEqualString(mHintList[i].objectName.cstr(), object_name_or_optional_id) ||
                    al::isEqualString(mHintList[i].optionalId.cstr(), object_name_or_optional_id) :
                al::isEqualSubString(mHintList[i].objectName.cstr(), object_name_or_optional_id) ||
                    al::isEqualString(mHintList[i].optionalId.cstr(), object_name_or_optional_id);
        if (!matches)
            continue;
        if (mHintList[i].isGet) {
            has_shine[mHintList[i].worldId] = true;
            count++;
        }
    }
    return count;
}

s32 GameDataFile::calcGetShineNumByObjectNameWithWorldId(const char* object_name,
                                                         s32 world_id) const {
    s32 count = 0;
    for (s32 i = 0; i < 1024; i++)
        if (mHintList[i].worldId == world_id &&
            al::isEqualString(mHintList[i].objectName.cstr(), object_name) && mHintList[i].isGet)
            count++;
    return count;
}

s32 GameDataFile::calcAllShineNumByObjectNameOrOptionalId(
    const char* object_name_or_optional_id) const {
    s32 count = 0;
    for (s32 i = 0; i < 1024; i++)
        if (al::isEqualSubString(mHintList[i].objectName.cstr(), object_name_or_optional_id) ||
            al::isEqualString(mHintList[i].optionalId.cstr(), object_name_or_optional_id))
            count++;
    return count;
}

s32 GameDataFile::calcGetShineNumByStageName(const char* stage_name) const {
    s32 count = 0;
    for (s32 i = 0; i < 1024; i++)
        if (al::isEqualSubString(mHintList[i].stageName.cstr(), stage_name) && mHintList[i].isGet)
            count++;
    return count;
}

bool GameDataFile::tryFindAndInitShineInfoByOptionalId(ShineInfo* info, const char* optional_id) {
    for (s32 i = 0; i < 1024; i++) {
        if (!mHintList[i].optionalId.isEmpty() &&
            al::isEqualString(mHintList[i].optionalId.cstr(), optional_id)) {
            info->init(mHintList[i].stageName.cstr(), mHintList[i].objId.cstr(),
                       al::StringTmp<128>("ScenarioName_%s", mHintList[i].objId.cstr()).cstr(),
                       nullptr, ShineInfo::Type_Shine);
            return true;
        }
    }
    return false;
}

s32 GameDataFile::tryFindUniqueId(const ShineInfo* shine_info) const {
    s32 index = tryFindShineIndexImpl(mHintList.begin(), shine_info);
    return index == -1 ? -1 : mHintList[index].uniqueId;
}

s32 GameDataFile::findUnlockShineNumCurrentWorld(bool* out_is_game_clear) const {
    *out_is_game_clear = false;
    if (isUnlockedWorld(GameDataFunction::getWorldNum(mGameDataHolder) - 1))
        return -1;
    if (isGameClear()) {
        for (s32 i = 0; i < GameDataFunction::getWorldNum(mGameDataHolder); i++) {
            if (!isUnlockedWorld(i)) {
                *out_is_game_clear = true;
                return mGameDataHolder->findUnlockShineNum(out_is_game_clear, i);
            }
        }
    }
    return mGameDataHolder->findUnlockShineNum(out_is_game_clear, getCurrentWorldIdNoDevelop());
}

s32 GameDataFile::getMainScenarioNoCurrent() const {
    return mMainScenarioNo[getCurrentWorldIdNoDevelop()];
}

void GameDataFile::setMainScenarioNo(s32 scenario_no) {
    rs::trySavePrepoCompleteMainScenario(getCurrentWorldIdNoDevelop(), scenario_no, mPlayTimeTotal,
                                         mSaveDataIdForPrepo,
                                         mGameDataHolder->getPlayTimeAcrossFile());
    mMainScenarioNo[getCurrentWorldIdNoDevelop()] =
        sead::Mathi::max(scenario_no, mMainScenarioNo[getCurrentWorldIdNoDevelop()]);
}

bool GameDataFile::isCollectedBgm(const char* name, const char* situation_name) const {
    CollectBgmInfo info(name, situation_name);
    s32 index = mCollectBgmList.binarySearch(&info, compareCollectBgmInfo);
    return index >= 0 && mCollectBgmList[index]->isCollected;
}

const CollectBgm& GameDataFile::getCollectBgmByIndex(s32 index) const {
    return cCollectBgmList[index];
}

static bool isEqualStringOrNull(const char* a, const char* b) {
    return a ? b && al::isEqualString(a, b) : !b;
}

static bool trySetCollectedBgmNoBailout(const sead::PtrArray<GameDataFile::CollectBgmInfo>& list,
                                        const char* name, const char* situation_name) {
    GameDataFile::CollectBgmInfo info(name, situation_name);
    s32 index = list.binarySearch(&info, compareCollectBgmInfo);
    if (index < 0 || list[index]->isCollected)
        return false;
    list[index]->isCollected = true;
    return true;
}

bool GameDataFile::trySetCollectedBgm(const char* name, const char* situation_name) {
    bool no_bailout = false;
    if (trySetCollectedBgmNoBailout(mCollectBgmList, name, situation_name)) {
        rs::trySavePrepoGetCollectBgmEvent(name, situation_name, false, getCollectedBgmNum(),
                                           mPlayTimeTotal, mSaveDataIdForPrepo,
                                           mGameDataHolder->getPlayTimeAcrossFile());
        no_bailout = true;
    }
    s32 bailout_count = 0;
    for (s32 i = 0; i < cCollectBgmBailoutInfoTableSize; i++) {
        CollectBgmBailoutInfo* bailout_info = &cCollectBgmBailoutInfoTable[i];
        if (al::isEqualString(name, bailout_info->name1) &&
            isEqualStringOrNull(situation_name, bailout_info->situationName1) &&
            trySetCollectedBgmNoBailout(mCollectBgmList, bailout_info->name2,
                                        bailout_info->situationName2)) {
            rs::trySavePrepoGetCollectBgmEvent(
                bailout_info->name2, bailout_info->situationName2, true, getCollectedBgmNum(),
                mPlayTimeTotal, mSaveDataIdForPrepo, mGameDataHolder->getPlayTimeAcrossFile());
            bailout_count++;
        }
    }
    return no_bailout || bailout_count > 0;
}

s32 GameDataFile::getCollectedBgmNum() const {
    s32 count = 0;
    for (s32 i = 0; i < mCollectBgmList.size(); i++)
        if (mCollectBgmList[i]->isCollected)
            count++;
    return count;
}

s32 GameDataFile::getCollectedBgmMaxNum() const {
    return mCollectBgmList.capacity();
}

void GameDataFile::setGotShine(s32 index) {
    if (index < 0)
        return;
    setGotShine(&mHintList[index]);
}

void GameDataFile::setGotShine(const HintInfo* info) {
    ShineInfo shine_info;
    shine_info.init(info->stageName.cstr(), info->objId.cstr(), "", nullptr, ShineInfo::Type_Shine);
    setGotShine(&shine_info);
}

bool GameDataFile::isEnableOpenMoonRock(s32 world_id) const {
    if (world_id == GameDataFunction::getWorldIndexPeach() ||
        world_id == GameDataFunction::getWorldIndexSpecial1() ||
        world_id == GameDataFunction::getWorldIndexSpecial2())
        return false;
    // TODO deduplicate code
    return (isGameClear() &&
            mGameDataHolder->getWorldList()->isEqualClearMainScenarioNo(
                sead::Mathi::max(world_id, 0), mScenarioNo[sead::Mathi::max(world_id, 0)])) ||
           mGameDataHolder->getWorldList()->isEqualAfterEndingScenarioNo(
               sead::Mathi::max(world_id, 0), mScenarioNo[sead::Mathi::max(world_id, 0)]);
}

static void writeUniqObjInfo(al::ByamlWriter* writer, const UniqObjInfo* info) {
    writer->pushHash();
    writer->addString("StageName", info->getStageName());
    writer->addString("ObjId", info->getObjId());
    writer->pop();
}

static void writeSaveObjS32(al::ByamlWriter* writer, const GameDataFile::SaveObjS32* obj) {
    writer->pushHash();
    writer->addString("StageName", obj->objInfo.getStageName());
    writer->addString("ObjId", obj->objInfo.getObjId());
    writer->addInt("Value", obj->value);
    writer->pop();
}

static void writeCoinCollectInfo(al::ByamlWriter* writer,
                                 const GameDataFile::CoinCollectInfo* info) {
    if (info->uniqueId == -1)
        return;
    writer->pushHash();
    writer->addInt("UniqueId", info->uniqueId);
    writer->addBool("IsGet", info->isGet);
    writer->pop();
}

static void writeItemList(al::ByamlWriter* writer, const char* key,
                          const sead::FixedSafeString<64>* item_list) {
    writer->pushArray(key);
    for (s32 i = 0; i < 64; i++)
        writer->addString(item_list[i].cstr());
    writer->pop();
}

static void writeCollectBgmInfo(al::ByamlWriter* writer, const GameDataFile::CollectBgmInfo* info) {
    if (!info->name)
        return;
    writer->pushHash();
    writer->addBool("IsCollected", info->isCollected);
    writer->addString("BgmResourceName", info->name);
    if (info->situationName)
        writer->addString("BgmSituationName", info->situationName);
    writer->pop();
}

bool GameDataFile::tryWriteByByaml(sead::WriteStream* stream, sead::Heap* heap) const {
    al::ByamlWriter writer{heap, false};
    tryWriteByByaml(&writer);
    stream->writeS32(writer.calcPackSize());
    writer.write(stream);
    return true;
}

bool GameDataFile::tryWriteByByaml(al::ByamlWriter* writer) const {
    writer->pushHash();
    writer->addInt("StartShineIndex", mStartShineIndex);
    writer->addString("PlayerStartIdForSave", mPlayerStartIdForSave.cstr());
    writer->addUInt64("LastUpdateTime", mSaveTime);
    writer->addInt64("SaveDataIdForPrepo", mSaveDataIdForPrepoForWrite);

    writer->pushArray("ShineData");
    for (s32 i = 0; i < 1024; i++) {
        if (mHintList[i].stageName.isEmpty())
            continue;
        writer->pushHash();
        writer->addInt("Status", mHintList[i].status);
        writer->addInt("UniqueId", mHintList[i].uniqueId);
        writer->addInt("HintStatus", mHintList[i].hintStatus);
        writer->addBool("IsGet", mHintList[i].isGet);
        writer->addUInt64("GetTime", mHintList[i].getTime);
        writer->pop();
    }
    writer->pop();

    mShopTalkData->write(writer);

    writer->pushArray("StartedObj");
    for (s32 i = 0; i < 512; i++) {
        if (mStartedObj[i].isEmpty())
            break;
        writeUniqObjInfo(writer, &mStartedObj[i]);
    }
    writer->pop();

    writer->pushArray("SaveObjS32");
    for (s32 i = 0; i < 256; i++) {
        if (mSaveObjS32[i].objInfo.isEmpty())
            break;
        writeSaveObjS32(writer, &mSaveObjS32[i]);
    }
    writer->pop();

    writer->pushArray("Checkpoint");
    for (s32 i = 0; i < 320; i++)
        writeUniqObjInfo(writer, &mGotCheckpoint[i]);
    writer->pop();

    writer->pushArray("WorldInfo");
    for (s32 i = 0; i < 20; i++) {
        writer->pushHash();
        writer->addInt("UnlockedWorldId", mUnlockedWorldId[i]);
        if (i == GameDataFunction::getWorldIndexCity() && mMainScenarioNo[i] <= 1)
            writer->addInt("MainShineNo", 0);
        else
            writer->addInt("MainShineNo", mMainScenarioNo[i]);
        writer->addInt("PayShineNum", mPayShineNum[i]);
        writer->addInt("UseCoinCollectNum", mUseCoinCollectNum[i]);
        writer->addInt("ScenarioNo", mScenarioNo[i]);
        writer->addInt("ShopShine", mShopShineNum[i]);
        writer->addBool("TalkLocalLanguage", mIsTalkLocalLanguage[i]);
        writer->addBool("WorldWarpHoleThrough", mIsWorldWarpHoleThrough[i]);
        writer->pop();
    }
    writer->pop();

    writer->pushArray("GetCoinCollectData");
    for (s32 i = 0; i < mCoinCollectList.capacity(); i++)
        writeCoinCollectInfo(writer, mCoinCollectList[i]);
    writer->pop();

    writer->addInt64("PlayTimeTotal", mPlayTimeTotal);
    if (mNextStageName.isEmpty())
        writer->addString("StageName", mCurrentStageName.cstr());
    else
        writer->addString("StageName", mNextStageName.cstr());
    writer->addInt("WorldId", mCurrentWorldIdForWrite);
    writer->addString("CheckpointName", mCheckpointName.cstr());
    writer->addInt("CoinNum", mCoinNum);
    writer->addInt("TotalCoinNum", mTotalCoinNum);
    writer->addInt("PlayerJumpCount", mPlayerJumpCount);
    writer->addInt("PlayerThrowCapCount", mPlayerThrowCapCount);

    writeItemList(writer, "ItemCap", mItemCap.begin());
    writeItemList(writer, "ItemCloth", mItemCloth.begin());
    writeItemList(writer, "ItemSticker", mItemSticker.begin());
    writeItemList(writer, "ItemGift", mItemGift.begin());

    writer->pushArray("HackDictionary");
    for (s32 i = 0; i < 160; i++)
        writer->addString(mHackDictionary[i].cstr());
    writer->pop();

    writer->addBool("IsRideSphinx", mIsRideSphinx);
    writer->addBool("IsRideMotorcycle", mIsRideMotorcycle);

    writer->pushArray("Session");
    for (s32 i = 0; i < SessionMusicianType::size(); i++)
        if (mIsExistSessionMember[i])
            writer->addInt(i);
    writer->pop();

    writer->addInt("SessionEventProgress", mSessionEventProgress);
    writer->addBool("IsPayCoinToSphinx", mIsPayCoinToSphinx);
    writer->addBool("IsHintNpcFirstTalk", mIsHintNpcFirstTalk);
    writer->addBool("IsKinopioBrigadeNpcFirstTalk", mIsKinopioBrigadeNpcFirstTalk);
    writer->addInt("CapStatusForJango", static_cast<s32>(mCapStatusForJango));

    mTimeBalloonSaveData->write(writer);

    writer->addString("WorldTravelingStatus", mWorldTravelingStatus.cstr());
    writer->addBool("IsStartWorldTravelingPeach", mIsStartWorldTravelingPeach);
    writer->addBool("IsPlayAlreadyWorldWarp", mIsPlayAlreadyWorldWarp);
    writer->addBool("IsTalkFirstAmiiboNpc", mIsTalkFirstAmiiboNpc);
    writer->addBool("IsUnlockAchievement", mIsUnlockAchievement);
    writer->addBool("IsPayShineAllInAllWorld", mIsPayShineAllInAllWorld);

    mWorldWarpTalkData->write(writer);
    mVisitStageData->write(writer);
    mGameProgressData->write(writer);
    mMoonRockData->write(writer);
    mBossSaveData->write(writer);
    mAchievementSaveData->write(writer);
    mSearchAmiiboDataTable->write(writer);
    mNetworkUploadFlag->write(writer);
    mSequenceDemoSkipData->write(writer);
    mHintPhotoData->write(writer);
    mPlayerHitPointData->write(writer);

    if (!mCurrentCostumeName.isEmpty())
        writer->addString("CurrentClothName", mCurrentCostumeName.cstr());
    if (!mCurrentCapName.isEmpty())
        writer->addString("CurrentCapName", mCurrentCapName.cstr());
    writer->addBool("IsCostumeRandom", mIsCostumeRandom);
    writer->addBool("IsCapRandom", mIsCapRandom);
    writer->addBool("IsPlayDemoOpening", mIsPlayDemoOpening);
    writer->addBool("IsEnableCap", mIsMeetCap);
    writer->addInt("NextWorldId", mNextWorldId);
    writer->addInt("PrevWorldId", mPrevWorldId);
    writer->addInt("JangoCount", mJangoCount);

    writer->pushHash("RaceRecord");
    for (s32 i = 0; i < mRaceRecord.size(); i++)
        mRaceRecord[i]->write(writer);
    writer->pop();

    writer->addInt("RaceLoseCountLv1", mRaceLoseCountLv1);
    writer->addInt("RaceLoseCountLv2", mRaceLoseCountLv2);

    writer->pushArray("SphinxQuizData");
    mSphinxQuizData->write(writer);
    writer->pop();

    writer->addInt("JumpingRopeBestCount", mJumpingRopeBestCount);
    writer->addInt("VolleyballBestCount", mVolleyballBestCount);
    writer->addBool("IsExistRecordJumpingRope", mIsExistRecordJumpingRope);
    writer->addBool("IsExistRecordVolleyball", mIsExistRecordVolleyball);
    writer->addBool("IsPlayDemoPlayerDownForBattleKoopaAfter",
                    mIsPlayDemoPlayerDownForBattleKoopaAfter);
    writer->addBool("IsKidsMode", mIsKidsMode);

    writer->pushArray("FlagTalkMessage");
    for (s32 i = 0; i < 256; i++)
        writer->addBool(mFlagTalkMessage[i]);
    writer->pop();

    writer->pushArray("GrowPot");
    for (s32 i = 0; i < 16; i++) {
        writer->pushHash();
        writer->addString("PotObj", mGrowFlower[i].potObj.cstr());
        writer->addString("SeedObj", mGrowFlower[i].seedObj.cstr());
        writer->addUInt64("Time", mGrowFlower[i].time);
        writer->addUInt("AddGrow", mGrowFlower[i].growLevel);
        writer->pop();
    }
    writer->pop();

    writer->addBool("ShowExplainCheckpointFlag", mIsShowExplainCheckpointFlag);

    writer->pushArray("CollectBgmList");
    for (s32 i = 0; i < mCollectBgmList.size(); i++)
        if (mCollectBgmList[i]->isCollected)
            writeCollectBgmInfo(writer, mCollectBgmList.unsafeAt(i));
    writer->pop();

    writer->addBool("IsTalkKakku", mIsTalkKakku);
    writer->addBool("IsTalkWorldTravelingPeach", mIsTalkWorldTravelingPeach);
    writer->addInt("TokimekiMayorNpcFavorabilityRating", mTokimekiMayorNpcFavorabilityRating);
    writer->addBool("FirstNetwork", mIsFirstNetwork);
    writer->pop();

    return true;
}
