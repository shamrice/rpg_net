#include "CommandProcessor.h"

CommandProcessor::CommandProcessor(std::string serverKey) {     
    this->serverKey = serverKey;
}

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

CommandTransaction* CommandProcessor::buildTransaction(IPaddress ip, const char *data) {
     
    const char *host = SDLNet_ResolveIP(&ip);
    Uint32 ipNum = SDL_SwapBE32(ip.host);
    Uint16 port = SDL_SwapBE16(ip.port);
                
    std::string dataString(data);  
    std::string hostString(host);     

    std::unordered_map<std::string, std::string> builtParameters;              

    try {
        //build transaction based on request
        Logger::write(Logger::LogLevel::DEBUG, "Command Processor : Raw Data string: " + dataString);
    
        //verify server key sent is correct
        std::string key = "";
        std::size_t keyPos = dataString.find(serverKey);

        if (keyPos != std::string::npos) {            
            key = dataString.substr(keyPos, serverKey.length());
            Logger::write(Logger::LogLevel::DEBUG, "Command Processor : server key=" + key + " keyPos=" + std::to_string(keyPos));
        }        
        if (key != serverKey) {
            Logger::write(Logger::LogLevel::ERROR, "Command Processor : Request server key does not match. Key supplied: " 
                + key + ". Returning NULL.");
            return NULL;
        }

        //get command string from request.
        std::size_t cmdStart = dataString.find_last_of(CommandConstants::SERVER_KEY_DELIMITER) + 1;
        std::size_t cmdEnd = dataString.find_first_of(CommandConstants::COMMAND_DELIMITER);
        int cmdLength = cmdEnd - cmdStart;
        std::string commandStr = dataString.substr(cmdStart, cmdLength); 
        
        //get params
        builtParameters = buildParameters(dataString); 

        Logger::write(Logger::LogLevel::DEBUG, "Command Processor : Data string: " + dataString 
                    + " cmd: " + commandStr);

        //get user's listening port from registration if not an add command.
        //int port = -1;
        if (commandStr != CommandConstants::ADD_COMMAND) {
            std::string username = builtParameters.at(CommandConstants::USER_KEY);
            Registration *userReg = GameState::getInstance().getRegistration(username);
            if (userReg != NULL) {
                port = userReg->getPort();
            } else {
                throw std::runtime_error("User registration not found");
            }
        }

        //execute command      
        // TODO : this is gross code. refactor this.  
        
        if (commandStr == "exit" || commandStr == "quit") {
            return new CommandTransaction(CommandType::SHUTDOWN, hostString, port, builtParameters);
        }

        if (commandStr == CommandConstants::UPDATE_COMMAND) {  
            Logger::write(Logger::LogLevel::INFO, "Command Processor : building update request");          
            return new CommandTransaction(CommandType::UPDATE, hostString, port, builtParameters);
        }

        if (commandStr == CommandConstants::ADD_COMMAND) {
            Logger::write(Logger::LogLevel::INFO, "Command Processor : building add request");            
            return new CommandTransaction(CommandType::ADD, hostString, port, builtParameters);
        
        }

        if (commandStr == CommandConstants::GET_COMMAND) {
            Logger::write(Logger::LogLevel::INFO, "Command Processor : building get request");                
            return new CommandTransaction(CommandType::GET, hostString, port, builtParameters);
        
        }

        if (commandStr == CommandConstants::LIST_COMMAND) {
            Logger::write(Logger::LogLevel::INFO, "Command Processor : building list request");                
            return new CommandTransaction(CommandType::LIST, hostString, port, builtParameters);
        }

        if (commandStr == CommandConstants::NOTIFICATION_COMMAND) {
            Logger::write(Logger::LogLevel::INFO, "Command Processor : building notification request");                
            return new CommandTransaction(CommandType::NOTIFICATION, hostString, port, builtParameters);
        }

    } catch (...) {
        Logger::write(Logger::LogLevel::INFO, "Command Processor : malformed client request. returning null");
        return NULL; 
    }

    //return null if transaction is invalid.
    Logger::write(Logger::LogLevel::INFO, "Command Processor : malformed client request. returning null");
    return NULL;
}

CommandTransaction* CommandProcessor::buildInfoTransactionResponse(IPaddress destIp, int statusCode, std::string message, bool isSuccess) {

    //TODO : Currently ports are hard coded because client sends on a random port #.

    const char *host = SDLNet_ResolveIP(&destIp);
    Uint32 ipNum = SDL_SwapBE32(destIp.host);
    Uint16 port = 4556; //SDL_SwapBE16(destIp.port);
           
    std::string hostString(host);     
    std::unordered_map<std::string, std::string> params;

    return buildInfoTransactionResponse(hostString, port, statusCode, message, isSuccess, params);

}

//Overload method when no params are passed.
CommandTransaction* CommandProcessor::buildInfoTransactionResponse(std::string host, int port, int statusCode, std::string message, bool isSuccess) {
    std::unordered_map<std::string, std::string> params;
    return buildInfoTransactionResponse(host, port, statusCode, message, isSuccess, params);
}

CommandTransaction* CommandProcessor::buildInfoTransactionResponse(std::string host, int port, int statusCode, std::string message, bool isSuccess, std::unordered_map<std::string, std::string> params) {
    
    if (host.length() > 0 && port > 0) {

        //std::unordered_map<std::string, std::string> params;
        if (isSuccess) {
            params.insert({CommandConstants::STATUS_KEY, CommandConstants::STATUS_SUCCESS});
        } else {
            params.insert({CommandConstants::STATUS_KEY, CommandConstants::STATUS_FAILURE});
        }

        params.insert({CommandConstants::STATUS_CODE_KEY, std::to_string(statusCode)});

        if (message.length() > 0) {
            params.insert({CommandConstants::MESSAGE_KEY, message});
        }

        return new CommandTransaction(
            CommandType::INFO,
            host,
            port,
            params
        );
    } 

    Logger::write(Logger::LogLevel::INFO, "Command Processor : failed to build info response. returning null");
    return NULL;
}

std::unordered_map<std::string, std::string> CommandProcessor::buildParameters(std::string rawParamString) {

    std::unordered_map<std::string, std::string> resultParams;

    //only parse params between param delimiters. Anything past final param end delimiter
    //will be ignored.    
    std::size_t startLoc = rawParamString.find(CommandConstants::PARAMETERS_START_DELIMITER);
    std::size_t endLoc = rawParamString.find(CommandConstants::PARAMETERS_END_DELIMITER); 
    std::string parameters = rawParamString.substr(startLoc + 2, endLoc - startLoc);

    Logger::write(Logger::LogLevel::INFO, "Command Processor : Unprocessed Params: " + parameters);

    //iterate through and get each key value pair
    size_t paramPos = 0;
    std::string paramKeyValuePairString;
    while ((paramPos = parameters.find(CommandConstants::PARAMETER_END_DELIMITER)) != std::string::npos) {
        paramKeyValuePairString = parameters.substr(0, paramPos);                
        parameters.erase(0, paramPos + 2);

        //found params, add to param map.
        std::string key = paramKeyValuePairString.substr(
            0, 
            paramKeyValuePairString.find(CommandConstants::PARAMETER_KEY_DELIMITER)
        );

        std::string value = paramKeyValuePairString.substr(
            paramKeyValuePairString.find(CommandConstants::PARAMETER_KEY_DELIMITER) + 1,
            paramKeyValuePairString.length()
        );
        resultParams.insert({key, value});
    }

    //list params to for debug to make sure parsed correctly.
    for (auto p : resultParams) {
        Logger::write(Logger::LogLevel::DEBUG, "Command Processor : result key: " 
            + p.first + " result value: " + p.second);
    }

    return resultParams;

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

            return buildInfoTransactionResponse(
                cmd->getHost(),
                port, 
                ResponseConstants::SUCCESS_CODE,
                ResponseConstants::ADD_USER_SUCCESS_MSG,
                true
            );
            
        } catch (...) {
            Logger::write(Logger::LogLevel::INFO, "Command Processor : Exception thrown. Failed to add user to game.");

            //this will never be seen by client as port in cmd is invalid for add commands
            return buildInfoTransactionResponse(
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
    return buildInfoTransactionResponse(
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

                return buildInfoTransactionResponse(
                    cmd->getHost(),
                    cmd->getPort(),
                    ResponseConstants::SUCCESS_CODE,
                    ResponseConstants::GET_USER_SUCCESS_MSG,
                    true,
                    params
                );

            } 
            Logger::write(Logger::LogLevel::INFO, "Command Processor : User " + username + " was not found.");            

            return buildInfoTransactionResponse(
                cmd->getHost(),
                cmd->getPort(),
                ResponseConstants::NOT_FOUND_CODE,
                ResponseConstants::NOT_FOUND_MSG,
                false
            );
        } catch (...) {
            Logger::write(Logger::LogLevel::INFO, "Command Processor : Exception thrown. Failed to get user info.");
            return buildInfoTransactionResponse(
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

    return buildInfoTransactionResponse(
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

            return buildInfoTransactionResponse(
                cmd->getHost(),
                cmd->getPort(),
                ResponseConstants::SUCCESS_CODE,
                ResponseConstants::LIST_USER_SUCCESS_MSG,
                true,
                params
            );
            
        } catch (...) {
            Logger::write(Logger::LogLevel::INFO, "Command Processor : Exception thrown. Failed to get users.");
            return buildInfoTransactionResponse(
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

    return buildInfoTransactionResponse(
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

                return buildInfoTransactionResponse(
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

                return buildInfoTransactionResponse(
                    cmd->getHost(),
                    cmd->getPort(),
                    ResponseConstants::NOT_ACTIVE_CODE,
                    ResponseConstants::NOT_ACTIVE_MSG,
                    false
                );
            }               
        } catch (...) {
            Logger::write(Logger::LogLevel::INFO, "Command Processor : Exception thrown. Failed to get user info.");
            return buildInfoTransactionResponse(
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

    return buildInfoTransactionResponse(
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

                return buildInfoTransactionResponse(
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
            
            return buildInfoTransactionResponse(
                cmd->getHost(),
                cmd->getPort(),
                ResponseConstants::PROCESS_FAILURE_CODE,
                ResponseConstants::PROCESS_FAILURE_MSG,
                false
            );
        }
    } 

    return buildInfoTransactionResponse(
        cmd->getHost(),
        cmd->getPort(),
        ResponseConstants::INVALID_CODE,
        ResponseConstants::INVALID_MSG,
        false
    );
}