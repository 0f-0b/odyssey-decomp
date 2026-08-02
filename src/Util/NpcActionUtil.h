#pragma once

#include <prim/seadSafeString.h>

namespace al {
class LiveActor;
}
class TalkNpcParam;

namespace rs {
TalkNpcParam* initTalkNpcParam(al::LiveActor* actor, const char*);
void startNpcAction(al::LiveActor* actor, const char*);
void makeNpcActionName(sead::BufferedSafeString*, const al::LiveActor* actor, const char*);
void tryStartNpcActionIfNotPlaying(al::LiveActor* actor, const char* action_name);
bool isExistNpcAction(const al::LiveActor* actor, const char* action_name);
bool isPlayingNpcAction(const al::LiveActor* actor, const char* action_name);
bool isOneTimeNpcAction(const al::LiveActor* actor, const char* action_name);
bool isExistTalkNpcParamHolder(const al::LiveActor* actor);
bool isInvalidNpcScare(const TalkNpcParam* param);
bool checkEnableStartEventAndCancelReaction(al::LiveActor* actor, const TalkNpcParam* param);
}  // namespace rs
