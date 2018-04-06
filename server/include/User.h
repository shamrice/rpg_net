#ifndef __USER__
#define __USER__

#include <string>

class User {

    public:
        User(std::string name);
        std::string getUsername();
        void setX(int x);
        int getX();
        void setY(int y);
        int getY();
    
    private:
        std::string username;
        int x;
        int y;
};

#endif