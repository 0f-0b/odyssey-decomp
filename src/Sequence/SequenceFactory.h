#pragma once

namespace al {
class Sequence;
}

namespace SequenceFactory {
al::Sequence* createSequence(const char* name);
}
