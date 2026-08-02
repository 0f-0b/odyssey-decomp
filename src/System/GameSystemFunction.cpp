#include "System/GameSystemFunction.h"

#include "System/Application.h"
#include "System/RootTask.h"

namespace GameSystemFunction {

GameSystem* getGameSystem() {
    return Application::instance()->getRootTask()->getGameSystem();
}

}  // namespace GameSystemFunction
