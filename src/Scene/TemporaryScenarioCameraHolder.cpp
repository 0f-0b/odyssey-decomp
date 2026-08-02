#include "Scene/TemporaryScenarioCameraHolder.h"

TemporaryScenarioCameraHolder::TemporaryScenarioCameraHolder(ScenarioCameraRelationInfo* buffer,
                                                             s32)
    : mBuffer(buffer) {}

void TemporaryScenarioCameraHolder::registInfo(s32 id, al::CameraTicket* camera) {
    mBuffer[mSize].id = id;
    mBuffer[mSize].camera = camera;
    mSize++;
}

al::CameraTicket* TemporaryScenarioCameraHolder::tryFindEntranceCameraByShineId(s32 id) const {
    for (s32 i = 0; i < mSize; i++)
        if (mBuffer[i].id == id)
            return mBuffer[i].camera;
    return nullptr;
}
