#pragma once

#include <basis/seadTypes.h>

#include "ModeBalloon/RankingCategory.h"
#include "System/ByamlSave.h"

namespace al {
class ByamlIter;
class ByamlWriter;
}  // namespace al

class NetworkUploadFlag : public ByamlSave {
public:
    NetworkUploadFlag();
    void init();
    bool isNeedUpload(const RankingCategory& category) const;
    void upload(const RankingCategory& category);
    void updateScore(const RankingCategory& category);
    void write(al::ByamlWriter* writer) override;
    void read(const al::ByamlIter& save) override;

    struct Entry {
        bool isNeedUpload;
        u64 lastUploadTime;
    };

private:
    Entry* mFlags;
};

static_assert(sizeof(NetworkUploadFlag) == 0x10);
