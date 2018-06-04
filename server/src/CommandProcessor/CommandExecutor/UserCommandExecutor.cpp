#include "CommandProcessor/CommandExecutor/UserCommandExecutor.h"

/*
 * Private
 */
CommandTransaction* UserCommandExecutor::executeAdd() {
    Logger::write(Logger::LogLevel::INFO, "UserCommandExecutor : Adding new user to game.");    

    //make sure it's actually an add command.
    if (cmd->getCommandAction() == CommandAction::ADD) {
        try {
            std::string username = cmd->getParameters().at(CommandConstants::USER_KEY);  
            std::string portStr = cmd->getParameters().at(CommandConstants::PORT_KEY);

            int port = atoi(portStr.c_str());

            User *newUser = new User(username);

            Registration newUserReg(
                username,
                cmd->getHost(), 
                port, 
                newUser
            );

            //add user to the game and register them.
            GameState::getInstance().add<Registration>(newUserReg);

            for (auto regs : GameState::getInstance().getMany<Registration>()) {
                if (regs != NULL && regs->getUser() != NULL) {                                        
                    GameState::getInstance().add<Notification>(
                        Notification(
                            regs->getUsername(), 
                            "User " + username + " has been added to the game."
                        )
                    );                
                } else {
                    Logger::write(Logger::ERROR, " UserCommandExecutor : Add user + " + username 
                        + " failed to get registration after adding");
                }
            }
            
            Logger::write(Logger::LogLevel::INFO, "UserCommandExecutor : Added user " + username + " to game.");

            return transactionBuilder.buildResponse(
                cmd->getHost(),
                port,
                ResponseConstants::SUCCESS_CODE,
                ResponseConstants::ADD_USER_SUCCESS_MSG,
                true
            );
            
        } catch (...) {
            Logger::write(Logger::LogLevel::INFO, "UserCommandExecutor : Exception thrown. Failed to add user to game.");

            //this will never be seen by client as port in cmd is invalid for add commands
                        
            return transactionBuilder.buildResponse(
                cmd->getHost(),
                cmd->getPort(),
                ResponseConstants::PROCESS_FAILURE_CODE,
                ResponseConstants::PROCESS_FAILURE_MSG,
                false
            );
        }

    } else {
        Logger::write(Logger::LogLevel::ERROR, "UserCommandExecutor : Attempted to pass wrong command type to add command.");
    }

    //this will never be seen by client as port in cmd is invalid for add commands
    return transactionBuilder.buildResponse(        
        cmd->getHost(),
        cmd->getPort(),
        ResponseConstants::INVALID_CODE,
        ResponseConstants::INVALID_MSG,
        false
    );
}

CommandTransaction* UserCommandExecutor::executeGet() {
    Logger::write(Logger::LogLevel::INFO, "UserCommandExecutor : Get user info.");

    //make sure it's actually anget command.
    if (cmd->getCommandAction() == CommandAction::GET) {
        try {
            std::string username = cmd->getParameters().at(CommandConstants::GET_USER_KEY);        
            User *foundUser = GameState::getInstance().get<User>(username);
            bool regStatus = GameState::getInstance().getStatus<Registration>(username);
                        
            if (foundUser != NULL) {
                std::unordered_map<std::string, std::string> params;
                params.insert({CommandConstants::STATUS_KEY, CommandConstants::STATUS_SUCCESS});
                params.insert({CommandConstants::GET_USER_KEY, foundUser->getUsername()});
                params.insert({CommandConstants::X_KEY, std::to_string(foundUser->getX())});
                params.insert({CommandConstants::Y_KEY, std::to_string(foundUser->getY())});
                params.insert({CommandConstants::IS_ACTIVE_KEY, std::to_string(regStatus)});

                Logger::write(Logger::LogLevel::INFO, "UserCommandExecutor : Found user " + username + " in game.");

                return transactionBuilder.buildResponse(    
                    cmd->getHost(),
                    cmd->getPort(),
                    ResponseConstants::SUCCESS_CODE,
                    ResponseConstants::GET_USER_SUCCESS_MSG,
                    true,
                    params
                );

            } 
            Logger::write(Logger::LogLevel::INFO, "UserCommandExecutor : User " + username + " was not found.");            

            return transactionBuilder.buildResponse(    
                cmd->getHost(),
                cmd->getPort(),
                ResponseConstants::NOT_FOUND_CODE,
                ResponseConstants::NOT_FOUND_MSG,
                false
            );
        } catch (...) {
            Logger::write(Logger::LogLevel::INFO, "UserCommandExecutor : Exception thrown. Failed to get user info.");

            return transactionBuilder.buildResponse(
                cmd->getHost(),
                cmd->getPort(),
                ResponseConstants::PROCESS_FAILURE_CODE,
                ResponseConstants::PROCESS_FAILURE_MSG,
                false
            );
        }

    } else {
        Logger::write(Logger::LogLevel::ERROR, "UserCommandExecutor : Attempted to pass wrong command type to get command.");
    }

    return transactionBuilder.buildResponse(
        cmd->getHost(),
        cmd->getPort(),
        ResponseConstants::INVALID_CODE,
        ResponseConstants::INVALID_MSG,
        false
    );    
}

CommandTransaction* UserCommandExecutor::executeList() {
    Logger::write(Logger::LogLevel::INFO, "UserCommandExecutor : list users");

    //make sure it's actually a list command.
    if (cmd->getCommandAction() == CommandAction::LIST) {
        try {
            
            std::vector<User*> foundUsers = GameState::getInstance().getMany<User>();            
            std::unordered_map<std::string, std::string> params;

            for (auto it = foundUsers.begin(); it < foundUsers.end(); ++it) {                                
                std::string username = (*it)->getUsername();    
                bool regStatus = GameState::getInstance().getStatus<Registration>(username);

                params.insert({username + "." + CommandConstants::USER_KEY, username});
                params.insert({username + "." + CommandConstants::X_KEY, std::to_string((*it)->getX())});
                params.insert({username + "." + CommandConstants::Y_KEY, std::to_string((*it)->getY())});
                params.insert({username + "." + CommandConstants::IS_ACTIVE_KEY, std::to_string(regStatus)});
            }

            params.insert({CommandConstants::STATUS_KEY, CommandConstants::STATUS_SUCCESS});
            
            Logger::write(Logger::LogLevel::INFO, "UserCommandExecutor : Found " 
                            + std::to_string(foundUsers.size()) + " users in game.");

            return transactionBuilder.buildResponse(
                cmd->getHost(),
                cmd->getPort(),
                ResponseConstants::SUCCESS_CODE,
                ResponseConstants::LIST_USER_SUCCESS_MSG,
                true,
                params
            );
            
        } catch (...) {
            Logger::write(Logger::LogLevel::INFO, "UserCommandExecutor : Exception thrown. Failed to get users.");

            return transactionBuilder.buildResponse(
                cmd->getHost(),
                cmd->getPort(),
                ResponseConstants::PROCESS_FAILURE_CODE,
                ResponseConstants::PROCESS_FAILURE_MSG,
                false
            );
        }

    } else {
        Logger::write(Logger::LogLevel::ERROR, "UserCommandExecutor : Attempted to pass wrong command type to list command.");
    }

    return transactionBuilder.buildResponse(
        cmd->getHost(),
        cmd->getPort(),
        ResponseConstants::INVALID_CODE,
        ResponseConstants::INVALID_MSG,
        false
    );
}

CommandTransaction* UserCommandExecutor::executeUpdate() {
    Logger::write(Logger::LogLevel::INFO, "UserCommandExecutor : Update user info.");

    //make sure it's actually anget command.
    if (cmd->getCommandAction() == CommandAction::UPDATE) {
        try {
            std::string username = cmd->getParameters().at(CommandConstants::USER_KEY);              

            bool isActive = GameState::getInstance().getStatus<Registration>(username);

            if (isActive) {

                User *userUpdate = new User(username);

                //only update values that exist in command.
                if (cmd->getParameters().find(CommandConstants::X_KEY) !=  cmd->getParameters().end()) {
                    int x = atoi(cmd->getParameters().at(CommandConstants::X_KEY).c_str());
                    userUpdate->setX(x);
                }     

                if (cmd->getParameters().find(CommandConstants::Y_KEY) !=  cmd->getParameters().end()) {
                    int y = atoi(cmd->getParameters().at(CommandConstants::Y_KEY).c_str());
                    userUpdate->setY(y);
                }   
           
                //update user entry in GameState::getInstance().
                GameState::getInstance().update<User>(userUpdate);            
                Logger::write(Logger::LogLevel::INFO, "UserCommandExecutor : Updated user " + username + ".");

                //output params.
                std::unordered_map<std::string, std::string> params;
                params.insert({CommandConstants::STATUS_KEY, CommandConstants::STATUS_SUCCESS});

                return transactionBuilder.buildResponse(
                    cmd->getHost(),
                    cmd->getPort(),
                    ResponseConstants::SUCCESS_CODE,
                    ResponseConstants::UPDATE_USER_SUCCESS_MSG,
                    true,
                    params
                );  
            } else {
                Logger::write(Logger::LogLevel::INFO, "UserCommandExecutor : Cannot update user " 
                    + username + ". User is no longer active.");

                return transactionBuilder.buildResponse(
                    cmd->getHost(),
                    cmd->getPort(),
                    ResponseConstants::NOT_ACTIVE_CODE,
                    ResponseConstants::NOT_ACTIVE_MSG,
                    false
                );
            }               
        } catch (...) {
            Logger::write(Logger::LogLevel::INFO, "UserCommandExecutor : Exception thrown. Failed to get user info.");

            return transactionBuilder.buildResponse(
                cmd->getHost(),
                cmd->getPort(),
                ResponseConstants::PROCESS_FAILURE_CODE,
                ResponseConstants::PROCESS_FAILURE_MSG,
                false
            );
        }

    } else {
        Logger::write(Logger::LogLevel::ERROR, "UserCommandExecutor : Attempted to pass wrong command type to get command.");
    }

    return transactionBuilder.buildResponse(
        cmd->getHost(),
        cmd->getPort(),
        ResponseConstants::INVALID_CODE,
        ResponseConstants::INVALID_MSG,
        false
    );
}


CommandTransaction* UserCommandExecutor::executeNotification() {
    Logger::write(Logger::ERROR, "UserCommandExecutor : execute notification not implemented.");
    return NULL;
}
