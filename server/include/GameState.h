#ifndef __GAME_STATE__
#define __GAME_STATE__

#include <iostream>
#include <unordered_map>
#include <vector>
#include <mutex>
#include "Logger.h"
#include "User.h"
#include "Registration.h"
#include "Notification.h"

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
        Registration* getRegistration(std::string username);
        bool getUserRegistrationStatus(std::string username);

        void addNotification(Notification newNotification);
        Notification getNextNotification();

    private:   
        GameState();     
        std::unordered_map<std::string, Registration> registrationMap;
        std::vector<Notification> notificationQueue;
        std::mutex addUserMutex;
};

#endif