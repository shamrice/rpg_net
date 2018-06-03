#ifndef __GAME_STATE__
#define __GAME_STATE__

#include <iostream>
#include <unordered_map>
#include <queue>
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

        template<typename T> void add(T item) { }
        template<typename T> void update(T *item) { }        
        template<typename T> T* get(std::string itemName) { }        
        template<typename T> std::vector<T*> getMany() { }
        template<typename T> bool getStatus(std::string itemName) { }
        template<typename T> T getNext() { }        
        template<typename T> bool remove(std::string itemName) { }            

        void setServerKey(std::string serverKey);
        std::string getServerKey();

    private:   
        GameState();     
        std::unordered_map<std::string, Registration> registrationMap;
        std::queue<Notification> notificationQueue;
        std::mutex addUserMutex;
        std::string serverKey;
};

#endif