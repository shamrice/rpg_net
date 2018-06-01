
#include "CommandProcessor/CommandExecutor/CommandExecutor.h"

#include <iostream> //debug

template <> 
bool CommandExecutor::get<bool>() { 
    return false;
}

template<>
bool CommandExecutor::get<std::string>() {
    return true;
}

template<>
bool CommandExecutor::update<User>(User item) {

    std::cout << " Updating user: " << item.getUsername() << "\n";
    
    return true;

}