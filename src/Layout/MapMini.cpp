#include "Layout/MapMini.h"

#include "Library/Camera/CameraUtil.h"
#include "Library/Layout/LayoutActorUtil.h"
#include "Library/Layout/LayoutInitInfo.h"
#include "Library/Math/MathUtil.h"
#include "Library/Math/MatrixUtil.h"
#include "Library/Nerve/NerveSetupUtil.h"
#include "Library/Nerve/NerveUtil.h"
#include "Library/Player/PlayerUtil.h"

#include "Layout/MapLayoutFunction.h"
#include "Scene/QuestInfo.h"
#include "Scene/QuestInfoHolder.h"
#include "System/GameDataFunction.h"
#include "System/GameDataUtil.h"
#include "Util/PlayerUtil.h"

namespace {
NERVE_IMPL(MapMini, Wait);
NERVE_IMPL(MapMini, Appear);
NERVE_IMPL(MapMini, End);

NERVES_MAKE_NOSTRUCT(MapMini, Wait);
NERVES_MAKE_NOSTRUCT(MapMini, Appear);
NERVES_MAKE_NOSTRUCT(MapMini, End);
}  // namespace

MapMini::MapMini(const al::LayoutInitInfo& info, const al::PlayerHolder* player_holder)
    : al::LayoutActor("[ミニマップ]本体"), mPlayerHolder(player_holder) {
    al::initLayoutActor(this, info, "MapMini");

    mPlayerIcon = new al::LayoutActor("[ミニマップ]プレイヤーアイコン");
    al::initLayoutActor(mPlayerIcon, info, "MapMiniPlayer");
    mPlayerIcon->kill();

    mIcons.allocBuffer(10, nullptr);
    for (s32 i = 0; i < 10; i++) {
        al::LayoutActor* icon = new al::LayoutActor("[ミニマップ]アイコン");
        al::initLayoutActor(icon, info, "MapMiniIcon");
        icon->kill();
        mIcons.pushBack(icon);
    }

    initNerve(&Wait);
    kill();
}

void MapMini::appearSlideIn() {
    if (GameDataFunction::isMainStage(this)) {
        al::setNerve(this, &Appear);
        appear();
    }
}

void MapMini::end() {
    al::setNerve(this, &End);
}

bool MapMini::isEnd() const {
    if (al::isNerve(this, &End) && !isAlive())
        return true;
    return false;
}

void MapMini::exeAppear() {
    if (al::isFirstStep(this)) {
        al::startAction(this, "Appear");
        s32 hint_num = GameDataFunction::calcHintNum(this);
        s32 hint_moon_rock_num = GameDataFunction::calcHintMoonRockNum(this);
        s32 main_scenario_num_max = GameDataFunction::getMainScenarioNumMax(this);
        mIconNumMax = sead::Mathi::min(hint_num + hint_moon_rock_num + main_scenario_num_max, 10);
        for (s32 i = 0; i < 10; i++)
            mIcons[i]->kill();
    }
    if (al::isActionEnd(this))
        al::setNerve(this, &Wait);
}

static f32 modDegree(f32 value) {
    return al::modf(value + 360.0f, 360.0f) + 0.0f;
}

static sead::Vector2f getMapTrans(const MapMini* layout) {
    const al::Matrix43f& mtx = al::getPaneMtx(layout, "PicMap");
    return {mtx.m[0][3], mtx.m[1][3]};
}

void MapMini::exeWait() {
    if (al::isFirstStep(this)) {
        al::startAction(this, "Wait");
        mPlayerIcon->appear();
        al::startAction(mPlayerIcon, "Appear");
    }

    // sp+0x5c
    f32 tmp;

    // sp+0x40
    sead::Vector3f north = sead::Vector3f::zero;
    rs::tryCalcMapNorthDir(&north, this);

    // sp+0x30
    sead::Vector3f player_dir = sead::Vector3f::zero;
    rs::calcPlayerFrontDir(&player_dir, al::getPlayerActor(mPlayerHolder, 0));
    tmp = 0.0f;
    al::tryCalcAngleOnPlaneDegree(&tmp, north, player_dir, sead::Vector3f::ey);
    f32 direct_angle = modDegree(tmp);
    f32 direct_frames_per_degree = al::getActionFrameMax(mPlayerIcon, "State", "Direct") / 360.0f;
    al::startFreezeAction(mPlayerIcon, "State", direct_angle * direct_frames_per_degree, "Direct");

    // sp+0x20
    sead::Vector3f camera_dir = al::getCameraAt(this, 0) - al::getCameraPos(this, 0);
    tmp = 0.0f;
    al::tryCalcAngleOnPlaneDegree(&tmp, north, camera_dir, sead::Vector3f::ey);
    f32 view_angle = modDegree(tmp);
    f32 view_frames_per_degree = al::getActionFrameMax(mPlayerIcon, "State", "View") / 360.0f;
    al::startFreezeAction(mPlayerIcon, "State", view_angle * view_frames_per_degree, "View");

    // sp+0x10
    sead::Vector3f camera_dir2 = al::getCameraAt(this, 0) - al::getCameraPos(this, 0);
    tmp = al::calcAngleOnPlaneDegree(camera_dir2, north, sead::Vector3f::ey);
    _138 = tmp;
    calcNearHintTrans();
    sead::Vector2f map_trans = getMapTrans(this);
    al::setLocalTrans(mPlayerIcon, {map_trans.x, map_trans.y});

    if (al::isActionPlaying(mPlayerIcon, "Appear") && al::isActionEnd(mPlayerIcon))
        al::startAction(mPlayerIcon, "Wait");
}

void MapMini::exeEnd() {
    if (al::isFirstStep(this)) {
        al::startAction(this, "End");
        mPlayerIcon->kill();
        for (s32 i = 0; i < mIconNumMax; i++)
            if (!al::isDead(mIcons[i]))
                mIcons[i]->kill();
    }
    if (al::isActionEnd(this))
        kill();
}

static void mul(sead::Vector3f* vec, const sead::Matrix44f& mtx) {
    sead::Vector3f tmp = *vec;
    vec->x = mtx.m[0][0] * tmp.x + mtx.m[0][1] * tmp.y + mtx.m[0][2] * tmp.z + mtx.m[0][3];
    vec->y = mtx.m[1][0] * tmp.x + mtx.m[1][1] * tmp.y + mtx.m[1][2] * tmp.z + mtx.m[1][3];
    vec->z = mtx.m[2][0] * tmp.x + mtx.m[2][1] * tmp.y + mtx.m[2][2] * tmp.z + mtx.m[2][3];
}

SEAD_ENUM(IconState, Scenario, Hint, HintRock);

static s32 findSimilarIcon(const sead::Vector3f* pos_list, const IconState* state_list, s32 count,
                           const sead::Vector3f& pos, IconState state) {
    for (s32 i = 0; i < count; i++)
        if ((pos_list[i] - pos).length() < 5.0f && state_list[i] == state)
            return i;
    return -1;
}

extern "C" void addIcon(f32* dist_list, sead::Vector3f* pos_list, IconState* state_list,
                        s32* number_list, s32* count, const sead::Vector3f& pos, f32 player_x,
                        f32 player_z, s32 limit, IconState state) {
    f32 dist = sead::Mathf::sqrt(player_x * player_x + player_z * player_z);
    if (*count < limit) {
        s32 found = findSimilarIcon(pos_list, state_list, *count, pos, state);
        if (found != -1) {
            number_list[found]++;
            return;
        }
        s32 index = *count;
        dist_list[index] = dist;
        pos_list[index] = pos;
        state_list[index] = state;
        number_list[index] = 1;
        (*count)++;
    } else {
        f32 max_dist = dist_list[0];
        s32 index = 0;
        for (s32 i = 1; i < limit; i++) {
            if (max_dist < dist_list[i]) {
                max_dist = dist_list[i];
                index = i;
            }
        }
        if (max_dist > dist) {
            s32 found = findSimilarIcon(pos_list, state_list, limit, pos, state);
            if (found != -1) {
                number_list[found]++;
                return;
            }
            dist_list[index] = dist;
            pos_list[index] = pos;
            state_list[index] = state;
            number_list[index] = 1;
        }
    }
}

void MapMini::calcNearHintTrans() {
    const sead::Vector3f& player_pos = rs::getPlayerPos(al::getPlayerActor(mPlayerHolder, 0));

    s32 count = 0;
    // x24
    f32 dist_list[mIconNumMax];
    // x22
    sead::Vector3f pos_list[mIconNumMax];
    // x21
    IconState state_list[mIconNumMax];
    // x20
    s32 number_list[mIconNumMax];
    for (s32 i = 0; i < mIconNumMax; i++)
        number_list[i] = 1;
    for (s32 i = 0; i < mIconNumMax; i++)
        dist_list[i] = 100000000.0f;

    s32 hint_num = GameDataFunction::calcHintNum(this);
    for (s32 i = 0; i < hint_num; i++) {
        const sead::Vector3f& pos = GameDataFunction::calcHintTrans(this, i);
        addIcon(dist_list, pos_list, state_list, number_list, &count, pos, player_pos.x - pos.x,
                player_pos.z - pos.z, mIconNumMax, IconState::Hint);
    }

    s32 hint_moon_rock_num = GameDataFunction::calcHintMoonRockNum(this);
    for (s32 i = 0; i < hint_moon_rock_num; i++) {
        const sead::Vector3f& pos = GameDataFunction::calcHintTrans(this, i);
        addIcon(dist_list, pos_list, state_list, number_list, &count, pos, player_pos.x - pos.x,
                player_pos.z - pos.z, mIconNumMax, IconState::HintRock);
    }

    sead::Vector3f pos = sead::Vector3f::zero;
    if (GameDataFunction::isExistJango(this)) {
        const sead::Vector3f& pos = GameDataFunction::getJangoTrans(this);
        addIcon(dist_list, pos_list, state_list, number_list, &count, pos, player_pos.x - pos.x,
                player_pos.z - pos.z, mIconNumMax, IconState::Scenario);
    } else if (rs::isSequenceGoToNextWorld(this)) {
        const sead::Vector3f& pos = GameDataFunction::getHomeTrans(this);
        addIcon(dist_list, pos_list, state_list, number_list, &count, pos, player_pos.x - pos.x,
                player_pos.z - pos.z, mIconNumMax, IconState::Scenario);
    } else if (GameDataFunction::tryGetNextMainScenarioPos(&pos, this) &&
               rs::getActiveQuestNum(this) == 0) {
        addIcon(dist_list, pos_list, state_list, number_list, &count, pos, player_pos.x - pos.x,
                player_pos.z - pos.z, mIconNumMax, IconState::Scenario);
    } else {
        s32 quest_num = rs::getActiveQuestNum(this);
        const QuestInfo* const* quests = rs::getActiveQuestList(this);
        for (s32 i = 0; i < quest_num; i++) {
            if (!al::isEqualString("Sea", GameDataFunction::getWorldDevelopName(
                                              this, GameDataFunction::getCurrentWorldId(this))) ||
                quests[i]->getScenarioName().isEmpty()) {
                const sead::Vector3f& pos = quests[i]->getTrans();
                addIcon(dist_list, pos_list, state_list, number_list, &count, pos,
                        player_pos.x - pos.x, player_pos.z - pos.z, mIconNumMax,
                        IconState::Scenario);
            }
        }
    }

    class SafeBool {
    public:
        SafeBool() = default;

        SafeBool(bool value) : mValue(value) {}

        operator bool() const { return mValue; }

    private:
        bool mValue = false;
    };

    // 1f7b70:
    const sead::Matrix44f& view_proj = rs::getMapViewProjMtx(this);
    f32 inv_scale = 1.0f / rs::getMapProjMtx(this).m[0][0];
    sead::Vector2f map_trans = getMapTrans(this);
    sead::Vector3f proj_player_pos = player_pos;
    mul(&proj_player_pos, view_proj);
    SafeBool visible[mIconNumMax];

    for (s32 i = 0; i < count; i++) {
        // 1f7cb8:
        mul(&pos_list[i], view_proj);

        // 1f7d38:
        sead::Vector2f _sp_0x30 = {pos_list[i].x - proj_player_pos.x,
                                   pos_list[i].y - proj_player_pos.y};

        visible[i] = (_sp_0x30 * inv_scale).length() <= 2000.0f;
        if (!visible[i])
            continue;

        // 1f7d88:
        _sp_0x30 *= 0.02f;
        _sp_0x30 += map_trans;
        al::setLocalTrans(mIcons[i], _sp_0x30);

        if (al::isDead(mIcons[i]))
            mIcons[i]->appear();

        switch (state_list[i]) {
        case IconState::HintRock:
            al::startFreezeAction(mIcons[i], "HintRock", 0.0f, "State");
            break;
        case IconState::Hint:
            al::startFreezeAction(mIcons[i], "Hint", 0.0f, "State");
            break;
        case IconState::Scenario:
            al::startFreezeAction(mIcons[i], "Scenario", 0.0f, "State");
            break;
        }
    }

    // 1f7ee0:
    for (s32 i = 0; i < count; i++) {
        if (!visible[i])
            continue;
        if (number_list[i] == 1) {
            al::startAction(mIcons[i], "Off", "OnOff");
        } else {
            al::startAction(mIcons[i], "On", "OnOff");
            al::WStringTmp<32> text(u"%d", number_list[i]);
            al::setPaneString(mIcons[i], "TxtNumber", text.cstr());
        }
    }

    // 1f7fd8:
    for (s32 i = 0; i < mIconNumMax; i++)
        if (!visible[i] && !al::isDead(mIcons[i]))
            mIcons[i]->kill();
}
