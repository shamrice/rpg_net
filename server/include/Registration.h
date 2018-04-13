#ifndef __REGISTRATION__
#define __REGISTRATION__

#include <string>
#include <chrono>
#include <ctime>

class Registration {

    public:
        Registration(std::string username, std::string host, int port);
        std::string getUsername();
        std::string getHost();
        int getPort();
        void updateLastActive();
        void setInactive();
        bool isActive();
        std::time_t getLastActive();

    private:
        //const double timeoutSeconds = 30; ///600; //10 minutes for now.
        std::string username;
        std::string host;
        int port;
        bool isRegActive;
        std::time_t lastActive;

};

#endif