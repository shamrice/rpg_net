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
        GameState();
        void addUser(User *user);
        User* getUser(std::string username);  
        void updateUser(User *user);
        std::vector<User*> getUsers();
        void addRegistration(Registration reg);
        bool removeRegistration(std::string username);
        void removeInactiveRegistrations();
        std::vector<Registration> getActiveRegistrations();
        bool getUserRegistrationStatus(std::string username);

    private:        
        std::unordered_map<std::string, User*> userMap;
        std::unordered_map<std::string, Registration> registrationMap;
};

#endif