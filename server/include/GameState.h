#ifndef __GAME_STATE__
#define __GAME_STATE__

#include <iostream>
#include <unordered_map>
#include <vector>
#include "Logger.h"
#include "User.h"
#include "Registration.h"

class GameState {

    public:
        static GameState& getInstance() {
            static GameState instance;
            return instance;
        };

        GameState(GameState const&) = delete;
        void operator=(GameState const&) = delete;

        void registerUser(Registration reg);
        bool unregisterUser(std::string username);

        User* getUser(std::string username);  
        void updateUser(User *user);
        std::vector<User*> getUsers();

        std::vector<Registration> getRegistrations();
        bool getUserRegistrationStatus(std::string username);

    private:   
        GameState();     
        std::unordered_map<std::string, Registration> registrationMap;
};

#endif