#include "GameState.h"

GameState::GameState() { }


User* GameState::getUser(std::string username) {
    try {
        std::lock_guard<std::mutex> guard(addUserMutex);                    
        Registration userReg = registrationMap.at(username);
        return userReg.getUser();
    } catch (const std::out_of_range ex) {
        Logger::write(Logger::LogLevel::ERROR, "ERROR : GameState::getUser : User: " + username 
            + " : Out of range error: " + ex.what());
        return NULL;
    }
}

std::vector<User*> GameState::getUsers() {

    std::vector<User*> results;

    std::lock_guard<std::mutex> guard(addUserMutex);                    
    for (auto item : registrationMap) {
        results.push_back(item.second.getUser());        
    } 

    return results;
}

void GameState::updateUser(User *user) {

    //update user if found and values are in rnage

    try {
        if (user->getX() > 0 && user->getY() > 0) {
            std::lock_guard<std::mutex> guard(addUserMutex);                    
            int newX = user->getX();
            int newY = user->getY();
            registrationMap.at(user->getUsername()).getUser()->setXY(newX, newY);
            registrationMap.at(user->getUsername()).updateLastActive();            
        }
    } catch (std::out_of_range outOfRangeEx) {
        Logger::write(Logger::LogLevel::ERROR, "Failed to update user. User not found.");        
    }
}

void GameState::registerUser(Registration reg) {
    //only one thread can add to an unordered_map at the same time.
    std::lock_guard<std::mutex> guard(addUserMutex);                    
    registrationMap.emplace(reg.getUsername(), reg);
}

bool GameState::unregisterUser(std::string username) {
    std::lock_guard<std::mutex> guard(addUserMutex);                    
    if (registrationMap.erase(username) >= 1) 
        return true;  
        
    return false;
}

//get vector of registrations 
std::vector<Registration> GameState::getRegistrations() {
    std::vector<Registration> results;
    std::lock_guard<std::mutex> guard(addUserMutex);                    
    for (auto it : registrationMap) {
        results.push_back(it.second);        
    }

    return results;
}

//gets user's registration status. 
bool GameState::getUserRegistrationStatus(std::string username) {
    bool active = false;

    try {
        std::lock_guard<std::mutex> guard(addUserMutex);                    
        Registration userFound = registrationMap.at(username);
        active = userFound.isActive();

        //Removes user from game and registration if they are not active.
        //not sure if i want to keep this logic here....
        if (!active) {
            Logger::write(Logger::LogLevel::INFO, "GameState : User is no longer active, removing from userMap");            
            //userMap.erase(username);
            registrationMap.erase(username);
        }
    } catch (std::out_of_range outOfRangeEx) {
        Logger::write(Logger::LogLevel::ERROR, "GameState : Get user registration for " 
            + username + " not found. " + outOfRangeEx.what());
    }

    return active;
}

Registration* GameState::getRegistration(std::string username) {
    try {
        std::lock_guard<std::mutex> guard(addUserMutex);                    
        return &registrationMap.at(username);
    } catch (std::out_of_range oorEx) {
         Logger::write(Logger::LogLevel::ERROR, "GameState : Get user registration for " 
            + username + " not found. " + oorEx.what());
    }

    return NULL;
}

//add new notification to the beginning of the queue
void GameState::addNotification(Notification newNotification) {
    notificationQueue.insert(notificationQueue.begin(), newNotification);
}

//get last notification from the queue and remove it.
Notification GameState::getNextNotification() {
    if (!notificationQueue.empty()) {
        Notification result = notificationQueue.back();
        notificationQueue.pop_back();
        return result;
    }

    return Notification({}, {});
}