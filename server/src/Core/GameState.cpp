#include "Core/GameState.h"

GameState::GameState() { }

template<>
void GameState::add<Registration>(Registration item) {
    //only one thread can add to an unordered_map at the same time.
    std::lock_guard<std::mutex> guard(addUserMutex);                    
    registrationMap.emplace(item.getUsername(), item);
}

//add new notification to the queue
template<>
void GameState::add<Notification>(Notification item) {
    notificationQueue.push(item);
}


template<>
void GameState::update<User>(User *item) {
    //update user if found and values are in rnage

    try {
        if (item->getX() > 0 && item->getY() > 0) {
            std::lock_guard<std::mutex> guard(addUserMutex);                    
            int newX = item->getX();
            int newY = item->getY();
            registrationMap.at(item->getUsername()).getUser()->setXY(newX, newY);
            registrationMap.at(item->getUsername()).updateLastActive();            
        }
    } catch (std::out_of_range outOfRangeEx) {
        Logger::write(Logger::LogLevel::ERROR, "Failed to update user. User not found.");        
    }
}
     
template<>
User* GameState::get<User>(std::string itemName) {
    try {
        std::lock_guard<std::mutex> guard(addUserMutex);                    
        Registration userReg = registrationMap.at(itemName);
        return userReg.getUser();
    } catch (const std::out_of_range ex) {
        Logger::write(Logger::LogLevel::ERROR, "ERROR : GameState::getUser : User: " + itemName 
            + " : Out of range error: " + ex.what());
        return NULL;
    }
}

template<>
Registration* GameState::get<Registration>(std::string itemName) {
    try {
        std::lock_guard<std::mutex> guard(addUserMutex);                    
        return &registrationMap.at(itemName);
    } catch (std::out_of_range oorEx) {
         Logger::write(Logger::LogLevel::ERROR, "GameState : Get user registration for " 
            + itemName + " not found. " + oorEx.what());
    }

    return NULL;
}

//get notification from the queue and remove it.
template<>
Notification GameState::getNext<Notification>() {
    if (!notificationQueue.empty()) {
        Notification result = notificationQueue.front();
        notificationQueue.pop();
        return result;
    }

    return Notification({}, {});
}


template<>
bool GameState::remove<Registration>(std::string itemName) {
    std::lock_guard<std::mutex> guard(addUserMutex);                    
    if (registrationMap.erase(itemName) >= 1) 
        return true;  
        
    return false;
}

//get users registration status
template<>
bool GameState::getStatus<Registration>(std::string itemName) {
    bool active = false;

    try {
        std::lock_guard<std::mutex> guard(addUserMutex);                    
        Registration userFound = registrationMap.at(itemName);
        active = userFound.isActive();

        //Removes user from game and registration if they are not active.
        //not sure if i want to keep this logic here....
        if (!active) {
            Logger::write(Logger::LogLevel::INFO, "GameState : User is no longer active, removing from userMap");                        
            registrationMap.erase(itemName);
        }
    } catch (std::out_of_range outOfRangeEx) {
        Logger::write(Logger::LogLevel::ERROR, "GameState : Get user registration for " 
            + itemName + " not found. " + outOfRangeEx.what());
    }

    return active;
}

template<>
std::vector<User*> GameState::getMany<User>() {

    std::vector<User*> results;

    std::lock_guard<std::mutex> guard(addUserMutex);                    
    for (auto item : registrationMap) {
        results.push_back(item.second.getUser());        
    } 

    return results;
}

//get vector of registrations
template<>
std::vector<Registration*> GameState::getMany<Registration>() {
    std::vector<Registration*> results;
    std::lock_guard<std::mutex> guard(addUserMutex);                    
    for (auto it : registrationMap) {
        results.push_back(new Registration(it.second));        
    }

    return results;
}

void GameState::setServerKey(std::string serverKey) {
    this->serverKey = serverKey;
}

std::string GameState::getServerKey() {
    return serverKey;
}
