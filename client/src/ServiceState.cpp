#include "ServiceState.h"

void ServiceState::setIsRunning(bool state) {
   isServiceRunning = state;
}

bool ServiceState::isRunning() {
    return isServiceRunning;
}