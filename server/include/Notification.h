#ifndef __NOTIFICATION__
#define __NOTIFICATION__

#include <string>

class Notification {

    public:
        Notification(std::string to, std::string message);
        Notification(std::string from, std::string to, std::string message);

        std::string getFrom();
        std::string getTo();
        std::string getMessage();

    private:
        std::string from;
        std::string to;
        std::string message;

};

#endif