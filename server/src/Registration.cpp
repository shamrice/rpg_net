#include "Registration.h"

Registration::Registration(std::string username, std::string host, int port, User *user) {
    this->username = username;
    this->host = host;
    this->port = port;

    //set last active to time of registration.
    updateLastActive();

    if (user == NULL) {
        Logger::write(Logger::LogLevel::ERROR, "Registration : User cannot be null for username " + username);
        this->user = NULL;
        isRegActive = false;
    } else {
        this->user = user;
        isRegActive = true;
    }
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

User* Registration::getUser() {
    return user;
}