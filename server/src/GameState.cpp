#include "GameState.h"

GameState::GameState() { }

void GameState::addUser(User user) {
    userList.insert({user.getUsername(), user});
}

User GameState::getUser(std::string username) {
    return userList.at(username);
}

std::vector<User> GameState::getUsers() {

    std::vector<User> results;

    for (auto& item : userList) {
        results.push_back(item.second);        
    } 

    return results;
}