#include "Registration.h"

Registration::Registration(std::string username, std::string host, int port) {
    this->username = username;
    this->host = host;
    this->port = port;

    //set last active to time of registration.
    updateLastActive();

    isRegActive = true;
}

std::string Registration::getUsername() {
    return username;
}

std::string Registration::getHost() {
    return host;
}

int Registration::getPort() {
    return port;
}

//updates lastActive to current time.
void Registration::updateLastActive() {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    lastActive = std::chrono::system_clock::to_time_t(now);
}

void Registration::setInactive() {
    isRegActive = false;
}

bool Registration::isActive() {
    return isRegActive;
}

std::time_t Registration::getLastActive() {
    return lastActive;
}