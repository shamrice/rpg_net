#ifndef __ENGINE__
#define __ENGINE__

#include <iostream>
#include <curses.h>
#include <vector>
#include <chrono>
#include "Logger.h"
#include "UdpClientService.h"
#include "ClientConfiguration.h"
#include "User.h"

const int MASK_COLOR_PAIR_INDEX = 1;

class Engine {

    public:
        Engine(ClientConfiguration *clientConfig);
        virtual ~Engine();
        bool init();
        void start();
        void run();

    private:
        void populateOtherUsers();
        bool isInit;
        bool isRunning;
        User user;
        std::unordered_map<std::string, User> otherUsers;
        UdpClientService *clientService;
        ClientConfiguration *clientConfig;
        
};

#endif