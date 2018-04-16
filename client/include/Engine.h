#ifndef __ENGINE__
#define __ENGINE__

#include <iostream>
#include <curses.h>
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
        bool isInit;
        bool isRunning;
        User user;
        UdpClientService *clientService;
        ClientConfiguration *clientConfig;
        
};

#endif