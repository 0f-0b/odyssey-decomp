#include "Sequence/SequenceFactory.h"

#include "Library/Factory/Factory.h"

#include "Sequence/E3Sequence.h"
#include "Sequence/HakoniwaSequence.h"

namespace SequenceFactory {

static al::Sequence* createHakoniwaSequence(const char* name) {
    return new HakoniwaSequence(name);
}

static al::Sequence* createE3Sequence(const char* name) {
    return new E3Sequence(name);
}

const al::NameToCreator<al::Sequence* (*)(const char* name)> sSequenceFactoryEntries[] = {
    {"HakoniwaSequence", createHakoniwaSequence},
    {"E3Sequence", createE3Sequence},
};

static s32 getSequenceIndex(const char* name) {
    for (s32 i = 0; i < std::size(sSequenceFactoryEntries); i++)
        if (al::isEqualString(name, sSequenceFactoryEntries[i].name))
            return i;
    return 1;
}

al::Sequence* createSequence(const char* name) {
    return sSequenceFactoryEntries[getSequenceIndex(name)].creationFunction(name);
}

}  // namespace SequenceFactory
