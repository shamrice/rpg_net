#ifndef __USER__
#define __USER__

#include <string>

class User {

    public:
        User();
        void setUsername(std::string username);
        std::string getUsername();
        void move(int deltaX, int deltaY);
        int getX();
        int getY();
        void setX(int x);
        void setY(int y);
    
    private:
        std::string username;
        int x;
        int y;

};

#endif 