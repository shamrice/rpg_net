#include "User.h"

User::User() {
    this->username = "NOT_SET";
    x = 1;
    y = 1;
}

void User::setUsername(std::string username) {
    this->username = username;
}

std::string User::getUsername() {
    return username;
}

void User::move(int deltaX, int deltaY) {
    x += deltaX;
    y += deltaY;
}

int User::getX() {
    return x;
}

int User::getY() {
    return y;
}

void User::setX(int x) {
    this->x = x;
}

void User::setY(int y) {
    this->y = y;
}