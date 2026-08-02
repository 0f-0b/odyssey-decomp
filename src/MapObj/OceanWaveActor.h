#pragma once

#include "Library/LiveActor/LiveActor.h"

class OceanWaveActor : public al::LiveActor {
public:
    OceanWaveActor(const char* actorName);

private:
    void* _padding[0x12];
};

static_assert(sizeof(OceanWaveActor) == 0x198);
