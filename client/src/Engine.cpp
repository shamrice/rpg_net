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
    clientService->shutdown();
    endwin();    
}

bool Engine::init() {


    Logger::setLogType(clientConfig->getLogType());
    Logger::setLogLevel(clientConfig->getLogLevel());

    clientService = new UdpClientService(
        clientConfig->getServerHost(),
        clientConfig->getServerPort(),
        clientConfig->getClientPort()
    );

    clientService->init();

    //initial curses configuration
    initscr();
    keypad(stdscr, true);    
    cbreak();
    echo();

    if (has_colors()) {
        start_color();
        
        init_pair(MASK_COLOR_PAIR_INDEX, COLOR_BLACK, COLOR_BLACK);
        init_pair(2, COLOR_WHITE, COLOR_BLACK);
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

        wprintw(stdscr, "Enter username: ");    

        std::string username;
        char c = getch();

        while (c != '\n') {
            username.push_back(c);
            c = getch();
        }

        user.setUsername(username);
        
        if (clientService->sendAndWait(
            "|test|user-add>[{user:" + username + "}{port:" + std::to_string(clientConfig->getClientPort()) + "}]"
        )) {
            Logger::write(Logger::LogLevel::INFO, "Successfully added user to game.");
        

            clientService->sendAndWait(
                "|test|user-upd>[{user:" 
                + user.getUsername() 
                + "}{x:" + std::to_string(user.getX())
                + "}{y:" + std::to_string(user.getY())
                + "}]");           

            isRunning = true;

            std::thread networkThread(networkThreadHelper, this);

            run();

            if (networkThread.joinable()) {
                networkThread.join();
            }

        } else {
            Logger::write(Logger::LogLevel::ERROR, "Failed to add user to game.");
        }
        //TODO : Also spin up network polling thread as well.
    }
}

/* 
 * Handles the player input portion of the game engine. Will run until
 * engine isRunning = false.
 */
void Engine::run() {

    //ncurses config for in game.
    erase();       //clear screen
    refresh();     //refresh screen
    timeout(1);    //set input timeout.
    curs_set(0);   //hide input cursor.
 
    //populateOtherUsers();
    

    while (isRunning) {
        int c = -1;
        int num = 2;

        while (c != 'q') {
            
            bool hasMoved = false;

            //start fps timer
            __int64_t startms = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count();
            
            //get user input
            c = getch();

            int deltaX = 0;
            int deltaY = 0;

            if (c == KEY_RIGHT) {
                hasMoved = true;
                deltaX = 1;
            } 
            if (c == KEY_LEFT) {
                hasMoved = true;
                deltaX = -1;
            } 
            if (c == KEY_DOWN) {
                hasMoved = true;
                deltaY = 1;
            }   
            if (c == KEY_UP) {
                hasMoved = true;
                deltaY = -1;
            }    


            //only send update if player moved.
            if (hasMoved) {
                //draw player mask
                attrset(COLOR_PAIR(MASK_COLOR_PAIR_INDEX));     
                move(user.getY(), user.getX());
                wprintw(stdscr, "@");

                user.move(deltaX, deltaY);

                //draw player
                attrset(COLOR_PAIR(2)); 
                move(user.getY(), user.getX());
                wprintw(stdscr, "@"); 

                clientService->sendCommand(
                    "|test|user-upd>[{user:" 
                    + user.getUsername() 
                    + "}{x:" + std::to_string(user.getX())
                    + "}{y:" + std::to_string(user.getY())
                    + "}]");                
            }

 
            
            //regulate fps to value of MAX_FPS in header
            __int64_t endms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

            if (endms - startms < 1000 / MAX_FPS) {
                int sleepFor = (1000 / MAX_FPS) - (endms - startms);
                std::this_thread::sleep_for(std::chrono::milliseconds(sleepFor));      
            }

            //calculate actual FPS
            __int64_t actualEnd = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            __int64_t totalduration = actualEnd - startms;
            int fps = (1000 /totalduration);
            
            va_list args;            
            std::string testString = "FPS: " + std::to_string(fps);
            move(2, 50);
            vw_printw(stdscr, testString.c_str(), args);                      
       
        }

        isRunning = false;        
    }
    
}

/*
 * Method used by the network thread to poll/listen for network events not initiated by the user
 */
void* Engine::networkThread() {

    std::unordered_map<std::string, User> deltaMap;
    deltaMap.insert(otherUsers.begin(), otherUsers.end());

    while (isRunning) {

        populateOtherUsers();

        //if we get a bum result from server, list will be empty. do act on empty lists.
        if (!otherUsers.empty()) {

            for (auto u : otherUsers) {
                //don't draw if current player
                if (u.first != user.getUsername()) {
                    //if we find user, check if we need to draw
                    if (deltaMap.find(u.first) != deltaMap.end()) {
                        int maskX = deltaMap.at(u.first).getX();
                        int maskY = deltaMap.at(u.first).getY();
                        //if they moved, mask, update delta and draw
                        if (maskX != u.second.getX() || maskY != u.second.getY()) {

                            attrset(COLOR_PAIR(MASK_COLOR_PAIR_INDEX));
                            move(maskY, maskX);
                            wprintw(stdscr, "&");                    

                            deltaMap.at(u.first).setX(u.second.getX());
                            deltaMap.at(u.first).setY(u.second.getY());

                            attrset(COLOR_PAIR(2));
                            move(u.second.getY(), u.second.getX());
                            wprintw(stdscr, "&");
                        }
                    } else {
                        //user does not exist in delta map. insert and they will be drawn next
                        //iteration.
                        deltaMap.insert({u.first, u.second});
                    }                
                }            
            }        

            //if user does not exist in current user list but exists in delta. They have left the game.
            //and should be removed.
            std::vector<std::string> usersToRemove;
            for (auto du : deltaMap) {
                if (otherUsers.find(du.first) == otherUsers.end()) {
                    usersToRemove.push_back(du.first);

                    //mask user to remove.
                    attrset(COLOR_PAIR(MASK_COLOR_PAIR_INDEX));
                    move(du.second.getY(), du.second.getX());
                    wprintw(stdscr, "&");  

                }
            }

            //remove users.
            for (auto removeUser : usersToRemove) {
                deltaMap.erase(removeUser);
            }

        }

        //make sure we're not still masking next draw in other threads.
        attrset(COLOR_PAIR(2));
    }
}

void Engine::populateOtherUsers() {

    std::vector<User> results = clientService->getUserList("|test|user-list>[{user:" + user.getUsername() + "}]");

    otherUsers.clear();   

    for (User result : results) {
        otherUsers.insert({result.getUsername(), result});
    }
    
    Logger::write(Logger::LogLevel::INFO, "Successfully populated user list.");
    
}