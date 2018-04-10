#include "GameState.h"

GameState::GameState() { }

void GameState::addUser(User *user) {
    userMap.insert({user->getUsername(), user});
}

User* GameState::getUser(std::string username) {
    try {
        return userMap.at(username);
    } catch (const std::out_of_range ex) {
        Logger::write(Logger::LogLevel::ERROR, "ERROR : GameState::getUser : User: " + username 
            + " : Out of range error: " + ex.what());
        return NULL;
    }
}

std::vector<User*> GameState::getUsers() {

    std::vector<User*> results;

    for (auto item : userMap) {
        results.push_back(item.second);        
    } 

    return results;
}

void GameState::updateUser(User *user) {

    //update user if found and values are 
    //in range.
    auto it = userMap.find(user->getUsername());
    if (it != userMap.end()) {
        if (user->getX() > 0) {
            (*it).second->setX(user->getX());
        }
        if (user->getY() > 0) {
            (*it).second->setY(user->getY());
        }

        //update last active in registration
        auto regIt = registrationMap.find((std::string)user->getUsername());
        if (regIt != registrationMap.end()) {
            regIt->second.updateLastActive();
        }
    }
}

//add a new user to the registration
void GameState::addRegistration(Registration reg) {
    registrationMap.emplace(reg.getUsername(), reg);
}

//remove a single registration.  
bool GameState::removeRegistration(std::string username) {
    registrationMap.erase(username);
}

//removes inactive registrations from game state.
void GameState::removeInactiveRegistrations() {
    for (auto it : registrationMap) {
        if (!it.second.isActive()) {
            registrationMap.erase(it.first);
        }
    }
}

//get vector of registrations whose status is active
std::vector<Registration> GameState::getActiveRegistrations() {
    std::vector<Registration> results;

    for (auto it : registrationMap) {
        if (it.second.isActive()) {
            results.push_back(it.second);
        }
    }

    return results;
}

//gets user's registration status. 
bool GameState::getUserRegistrationStatus(std::string username) {
    bool active = false;

    try {
        Registration userFound = registrationMap.at(username);
        active = userFound.isActive();

        //Removes user from game and registration if they are not active.
        //not sure if i want to keep this logic here....
        if (!active) {
            Logger::write(Logger::LogLevel::INFO, "GameState : User is no longer active, removing from userMap");            
            userMap.erase(username);
            registrationMap.erase(username);
        }
    } catch (std::out_of_range outOfRangeEx) {
        Logger::write(Logger::LogLevel::ERROR, "GameState : Get user registration for " 
            + username + " not found. " + outOfRangeEx.what());
    }

    return active;
}