#include "CommandProcessor/CommandProcessor.h"

CommandTransaction* CommandProcessor::executeCommand(CommandTransaction *request) {

    CommandTransaction *response = NULL;

    switch (request->getCommandType()) {
        case UPDATE: {
            response = processUpdateCommand(request); 
        } break;
        
        case ADD: {
            response = processAddCommand(request);
        } break;

        case GET: {
            response = processGetCommand(request);
        } break;

        case LIST: {
            response = processListCommand(request);
        } break;

        case NOTIFICATION: {
            response = processNotificationCommand(request);
        } break;
    }

    if (response == NULL) {
        Logger::write(Logger::LogLevel::DEBUG, "Command Processor : Execute Command : Command type not found. Request after parsing was NULL");
    }
    //return null when response not needed.
    return response;
}

CommandTransaction* CommandProcessor::processAddCommand(CommandTransaction *cmd) {
    Logger::write(Logger::LogLevel::INFO, "Command Processor : Adding new user to game.");    

    //make sure it's actually an add command.
    if (cmd->getCommandType() == CommandType::ADD) {
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
            GameState::getInstance().registerUser(newUserReg);

            for (auto regs : GameState::getInstance().getRegistrations()) {
                
                GameState::getInstance().addNotification(
                    Notification(
                        regs.getUsername(), 
                        "User " + username + " has been added to the game."
                    )
                );
            }

            Logger::write(Logger::LogLevel::INFO, "Command Processor : Added user " + username + " to game.");

            return transactionBuilder.buildResponse(
                cmd->getHost(),
                port,
                ResponseConstants::SUCCESS_CODE,
                ResponseConstants::ADD_USER_SUCCESS_MSG,
                true
            );
            
        } catch (...) {
            Logger::write(Logger::LogLevel::INFO, "Command Processor : Exception thrown. Failed to add user to game.");

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
        Logger::write(Logger::LogLevel::ERROR, "Command Processor : Attempted to pass wrong command type to add command.");
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

CommandTransaction* CommandProcessor::processGetCommand(CommandTransaction *cmd) {
    Logger::write(Logger::LogLevel::INFO, "Command Processor : Get user info.");

    //make sure it's actually anget command.
    if (cmd->getCommandType() == CommandType::GET) {
        try {
            std::string username = cmd->getParameters().at(CommandConstants::GET_USER_KEY);        
            User *foundUser = GameState::getInstance().getUser(username);
            bool regStatus = GameState::getInstance().getUserRegistrationStatus(username);
                        
            if (foundUser != NULL) {
                std::unordered_map<std::string, std::string> params;
                params.insert({CommandConstants::STATUS_KEY, CommandConstants::STATUS_SUCCESS});
                params.insert({CommandConstants::GET_USER_KEY, foundUser->getUsername()});
                params.insert({CommandConstants::X_KEY, std::to_string(foundUser->getX())});
                params.insert({CommandConstants::Y_KEY, std::to_string(foundUser->getY())});
                params.insert({CommandConstants::IS_ACTIVE_KEY, std::to_string(regStatus)});

                Logger::write(Logger::LogLevel::INFO, "Command Processor : Found user " + username + " in game.");

                return transactionBuilder.buildResponse(    
                    cmd->getHost(),
                    cmd->getPort(),
                    ResponseConstants::SUCCESS_CODE,
                    ResponseConstants::GET_USER_SUCCESS_MSG,
                    true,
                    params
                );

            } 
            Logger::write(Logger::LogLevel::INFO, "Command Processor : User " + username + " was not found.");            

            return transactionBuilder.buildResponse(    
                cmd->getHost(),
                cmd->getPort(),
                ResponseConstants::NOT_FOUND_CODE,
                ResponseConstants::NOT_FOUND_MSG,
                false
            );
        } catch (...) {
            Logger::write(Logger::LogLevel::INFO, "Command Processor : Exception thrown. Failed to get user info.");

            return transactionBuilder.buildResponse(
                cmd->getHost(),
                cmd->getPort(),
                ResponseConstants::PROCESS_FAILURE_CODE,
                ResponseConstants::PROCESS_FAILURE_MSG,
                false
            );
        }

    } else {
        Logger::write(Logger::LogLevel::ERROR, "Command Processor : Attempted to pass wrong command type to get command.");
    }

    return transactionBuilder.buildResponse(
        cmd->getHost(),
        cmd->getPort(),
        ResponseConstants::INVALID_CODE,
        ResponseConstants::INVALID_MSG,
        false
    );
}

CommandTransaction* CommandProcessor::processListCommand(CommandTransaction *cmd) {
    Logger::write(Logger::LogLevel::INFO, "Command Processor : list users");

    //make sure it's actually a list command.
    if (cmd->getCommandType() == CommandType::LIST) {
        try {
            
            std::vector<User*> foundUsers = GameState::getInstance().getUsers();            
            std::unordered_map<std::string, std::string> params;

            for (auto it = foundUsers.begin(); it < foundUsers.end(); ++it) {                                
                std::string username = (*it)->getUsername();    
                bool regStatus = GameState::getInstance().getUserRegistrationStatus(username);

                params.insert({username + "." + CommandConstants::USER_KEY, username});
                params.insert({username + "." + CommandConstants::X_KEY, std::to_string((*it)->getX())});
                params.insert({username + "." + CommandConstants::Y_KEY, std::to_string((*it)->getY())});
                params.insert({username + "." + CommandConstants::IS_ACTIVE_KEY, std::to_string(regStatus)});
            }

            params.insert({CommandConstants::STATUS_KEY, CommandConstants::STATUS_SUCCESS});
            
            Logger::write(Logger::LogLevel::INFO, "Command Processor : Found " 
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
            Logger::write(Logger::LogLevel::INFO, "Command Processor : Exception thrown. Failed to get users.");

            return transactionBuilder.buildResponse(
                cmd->getHost(),
                cmd->getPort(),
                ResponseConstants::PROCESS_FAILURE_CODE,
                ResponseConstants::PROCESS_FAILURE_MSG,
                false
            );
        }

    } else {
        Logger::write(Logger::LogLevel::ERROR, "Command Processor : Attempted to pass wrong command type to list command.");
    }

    return transactionBuilder.buildResponse(
        cmd->getHost(),
        cmd->getPort(),
        ResponseConstants::INVALID_CODE,
        ResponseConstants::INVALID_MSG,
        false
    );
}


CommandTransaction* CommandProcessor::processUpdateCommand(CommandTransaction *cmd) {
    Logger::write(Logger::LogLevel::INFO, "Command Processor : Update user info.");

    //make sure it's actually anget command.
    if (cmd->getCommandType() == CommandType::UPDATE) {
        try {
            std::string username = cmd->getParameters().at(CommandConstants::USER_KEY);              

            bool isActive = GameState::getInstance().getUserRegistrationStatus(username);

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
                GameState::getInstance().updateUser(userUpdate);            
                Logger::write(Logger::LogLevel::INFO, "Command Processor : Updated user " + username + ".");

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
                Logger::write(Logger::LogLevel::INFO, "Command Processor : Cannot update user " 
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
            Logger::write(Logger::LogLevel::INFO, "Command Processor : Exception thrown. Failed to get user info.");

            return transactionBuilder.buildResponse(
                cmd->getHost(),
                cmd->getPort(),
                ResponseConstants::PROCESS_FAILURE_CODE,
                ResponseConstants::PROCESS_FAILURE_MSG,
                false
            );
        }

    } else {
        Logger::write(Logger::LogLevel::ERROR, "Command Processor : Attempted to pass wrong command type to get command.");
    }

    return transactionBuilder.buildResponse(
        cmd->getHost(),
        cmd->getPort(),
        ResponseConstants::INVALID_CODE,
        ResponseConstants::INVALID_MSG,
        false
    );
}

CommandTransaction* CommandProcessor::processNotificationCommand(CommandTransaction *cmd) {

    if (cmd == NULL) {
        Logger::write(Logger::LogLevel::ERROR, "CommandProcessor : processNotificationCommand received NULL cmd.");
        return NULL;
    }

    if (cmd->getCommandType() == CommandType::NOTIFICATION) {

        try {
            std::string to = cmd->getParameters().at(CommandConstants::NOTIFICATION_TO_KEY);
            std::string from = cmd->getParameters().at(CommandConstants::USER_KEY);
            std::string message = cmd->getParameters().at(CommandConstants::NOTIFICATION_MESSAGE_KEY);     
        
            Registration *toReg = GameState::getInstance().getRegistration(to);

            //update chat senders last activity time.
            GameState::getInstance().getRegistration(from)->updateLastActive();
            
            //make sure to only send message if user exists in registry.
            if (toReg != NULL) {

                Notification noteToSend(from, toReg->getUsername(), message);
                GameState::getInstance().addNotification(noteToSend);
             
                Logger::write(Logger::LogLevel::INFO, "Command Processor : Adding notification from " 
                    + from + " to " + to + " with message " + message);

                return transactionBuilder.buildResponse(    
                    cmd->getHost(),
                    cmd->getPort(),
                    ResponseConstants::SUCCESS_CODE,
                    ResponseConstants::NOTIFICATION_SUCCESS_MSG,
                    true
                ); 
            }
        } catch (std::out_of_range outOfRangeEx) {
            Logger::write(Logger::LogLevel::ERROR, "Command Processor : Exception thrown. Failed to process notification command "); 
            std::cerr << "Command Processor : processNotificationCommand : Exception = " << outOfRangeEx.what() << std::endl;
            
            return transactionBuilder.buildResponse(
                cmd->getHost(),
                cmd->getPort(),
                ResponseConstants::PROCESS_FAILURE_CODE,
                ResponseConstants::PROCESS_FAILURE_MSG,
                false
            );
        }
    } 

    return transactionBuilder.buildResponse(
        cmd->getHost(),
        cmd->getPort(),
        ResponseConstants::INVALID_CODE,
        ResponseConstants::INVALID_MSG,
        false
    );
}