#include "Core/User.h"

User::User(std::string name) {
    username = name;
    x = 0;
    y = 0;
}

std::string User::getUsername() {
    return username;
}

void User::setX(int x) {
    this->x = x;
}

int User::getX() {
    return x;
}

void User::setY(int y) {
    this->y = y;
}

int User::getY() {
    return y;
}

void User::setXY(int x, int y) {
    this->x = x;
    this->y = y;
}