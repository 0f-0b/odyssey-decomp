#pragma once

namespace al {
class NexNetworkStateHolder;

class IUseNexNetworkStateHolder {
public:
    virtual NexNetworkStateHolder* getNexNetworkStateHolder() const = 0;
};

}  // namespace al
