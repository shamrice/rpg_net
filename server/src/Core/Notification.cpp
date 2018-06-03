#include "Core/Notification.h"

Notification::Notification(std::string to, std::string message) {
    this->from = "SERVER_MSG";
    this->to = to;
    this->message = message;
}


Notification::Notification(std::string from, std::string to, std::string message) {
    this->from = from;
    this->to = to;
    this->message = message;
}

std::string Notification::getFrom() {
    return from;
}
        
        
std::string Notification::getTo() {
    return to;
}


std::string Notification::getMessage() {
    return message;
}