#include "Engine.h"

Engine::Engine(ClientConfiguration *clientConfig) {

    if (clientConfig == NULL) {
        std::cerr << "FATAL ERROR: Client configuration cannot be NULL";
        exit(-1);
    }

    this->clientConfig = clientConfig;
    isInit = false;
    isRunning = false;
}

Engine::~Engine() {

    endwin();
}

bool Engine::init() {

    clientService = new UdpClientService(
        clientConfig->getServerHost(),
        clientConfig->getServerPort(),
        clientConfig->getClientPort()
    );

    //curses configuration
    initscr();
    keypad(stdscr, true);
    cbreak();
    echo();

    if (has_colors()) {
        start_color();
        
        init_pair(MASK_COLOR_PAIR_INDEX, COLOR_GREEN, COLOR_GREEN);
        init_pair(2, COLOR_RED, COLOR_GREEN);
        init_pair(3, COLOR_YELLOW, COLOR_GREEN);
        init_pair(4, COLOR_BLUE, COLOR_GREEN);
        init_pair(5, COLOR_CYAN, COLOR_GREEN);
        init_pair(6, COLOR_MAGENTA, COLOR_GREEN);
        init_pair(7, COLOR_BLACK, COLOR_BLACK);        

        bkgd(COLOR_PAIR(2));
    }

    isInit = true;
    return isInit;
}

/*
 * Handles starting the game engine and everything necessary to start running.
 * Once ready to run, calls run engine.
 */
void Engine::start() {
    if (isInit) {

        std::cout << "Enter username: ";
        std::string username;
        std::cin >> username;

        user.setUsername(username);

        isRunning = true;
        run();
        //TODO : Also spin up network polling thread as well.
    }
}

/* 
 * Handles the player input portion of the game engine. Will run until
 * engine isRunning = false.
 */
void Engine::run() {

    while (isRunning) {
        int c = -1;
        int num = 2;

        while (c != 'q') {
            c = getch();
            
            attrset(COLOR_PAIR(MASK_COLOR_PAIR_INDEX));     
            move(user.getY(), user.getX());
            wprintw(stdscr, "@");

            if (c == KEY_RIGHT) {
                user.move(1, 0);
            } 
            if (c == KEY_LEFT) {
                user.move(-1, 0);
            } 
            if (c == KEY_DOWN) {
                user.move(0, 1);
            }   
            if (c == KEY_UP) {
                user.move(0, -1);
            }        

            attrset(COLOR_PAIR(2)); 
            move(user.getY(), user.getX());
            wprintw(stdscr, "@");  
        }

        isRunning = false;
    }
}