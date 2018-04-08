#include "GameState.h"

GameState::GameState() { }

void GameState::addUser(User *user) {
    userList.insert({user->getUsername(), user});
}

User* GameState::getUser(std::string username) {
    try {
        return userList.at(username);
    } catch (const std::out_of_range ex) {
        std::cerr << "ERROR : GameState::getUser : User: " + username 
            + " : Out of range error: " << ex.what() << std::endl;
        return NULL;
    }
}

std::vector<User*> GameState::getUsers() {

    std::vector<User*> results;

    for (auto item : userList) {
        results.push_back(item.second);        
    } 

    return results;
}

void GameState::updateUser(User *user) {

    //update user if found and values are 
    //in range.
    auto it = userList.find(user->getUsername());
    if (it != userList.end()) {
        if (user->getX() > 0) {
            (*it).second->setX(user->getX());
        }
        if (user->getY() > 0) {
            (*it).second->setY(user->getY());
        }
    }
}