#pragma once

#include "Library/Sequence/Sequence.h"

class E3Sequence : public al::Sequence {
public:
    E3Sequence(const char* name);

private:
    void* _padding[0x53];
};

static_assert(sizeof(E3Sequence) == 0x348);
