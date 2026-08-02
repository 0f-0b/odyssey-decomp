#pragma once

#include <basis/seadTypes.h>

#include "Library/HostIO/HioNode.h"
#include "Library/Network/IUseNexNetworkStateHolder.h"

namespace nn {
namespace account {
class UserHandle;
}

namespace nex {
class NgsFacade;
}
}  // namespace nn

namespace sead {
class Thread;
}

namespace al {

class NetworkSystem : public IUseNexNetworkStateHolder, public HioNode {
public:
    NetworkSystem(nn::account::UserHandle*, bool);
    virtual ~NetworkSystem();

    void allocNexMemory(u64);
    void freeNexMemory(void*);
    void initializeNex();
    void finalizeNex();
    void execute(sead::Thread*, s64);
    nn::nex::NgsFacade* getNgsFacade() const;
    void endInit();
    NexNetworkStateHolder* getNexNetworkStateHolder() const override;
    void updateBeforeScene();
    void updateAfterScene();
    void requestSystemInitialize();
    bool isAvailable() const;

private:
    static void* sNexHeap;
    void* _padding[0x8];
};

static_assert(sizeof(NetworkSystem) == 0x48);

}  // namespace al
