#ifndef __GAME_STATE__
#define __GAME_STATE__

#include <unordered_map>
#include <vector>
#include "User.h"

class GameState {

    public:
        GameState();
        void addUser(User user);
        User getUser(std::string username);  
        std::vector<User> getUsers();

    private:
        std::unordered_map<std::string, User> userList;
};

#endif