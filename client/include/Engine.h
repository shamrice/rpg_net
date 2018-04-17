#ifndef __ENGINE__
#define __ENGINE__

#include <iostream>
#include <curses.h>
#include <vector>
#include <chrono>
#include <stdarg.h>
#include <thread>
#include "Logger.h"
#include "UdpClientService.h"
#include "ClientConfiguration.h"
#include "User.h"

const int MASK_COLOR_PAIR_INDEX = 1;
const int MAX_FPS = 25; 

class Engine {

    public:
        static void *networkThreadHelper(void *context) {
            return ((Engine *)context)->networkThread();
        }

        Engine(ClientConfiguration *clientConfig);
        virtual ~Engine();
        bool init();
        void start();
        void run();
        void* networkThread();

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