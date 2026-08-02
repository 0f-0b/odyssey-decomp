#pragma once

namespace al {

class IEventFlowQueryJudge {
public:
    virtual const char* judgeQuery(const char* judge_name) const = 0;
};

}  // namespace al
