#include "CommandProcessor.h"

CommandProcessor::CommandProcessor(std::string serverKey) { 
    //temp having it's own logger...
    this->serverKey = serverKey;
    log = new Logger(Logger::LogType::CONSOLE);
}

CommandTransaction* CommandProcessor::executeCommand(CommandTransaction *request) {
    //debug bs right now

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

    }

    if (response == NULL) {
        log->write(Logger::LogLevel::DEBUG, "Command Processor : Execute Command : Command type not found");
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
        log->write(Logger::LogLevel::DEBUG, "Command Processor : Raw Data string: " + dataString);
    
        //verify server key sent is correct
        std::string key = "";
        std::size_t keyPos = dataString.find(serverKey);

        if (keyPos != std::string::npos) {            
            key = dataString.substr(keyPos, serverKey.length());
            log->write(Logger::LogLevel::DEBUG, "Command Processor : key=" + key + " keyPos=" + std::to_string(keyPos));
        }        
        if (key != serverKey) {
            log->write(Logger::LogLevel::ERROR, "Command Processor : Request server key does not match. Key supplied: " 
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

        log->write(Logger::LogLevel::DEBUG, "Command Processor : Data string: " + dataString 
                    + " cmd: " + commandStr);

        //execute command
        //TODO : currently ports are hard coded!
        if (commandStr == "exit" || commandStr == "quit") {
            return new CommandTransaction(CommandType::SHUTDOWN, hostString, port, builtParameters);
        }

        if (commandStr == "upd") {  
            log->write(Logger::LogLevel::INFO, "Command Processor : building update request");          
            return new CommandTransaction(CommandType::UPDATE, hostString, 4556, builtParameters);
        }

        if (commandStr == "add") {
            log->write(Logger::LogLevel::INFO, "Command Processor : building add request");            
            return new CommandTransaction(CommandType::ADD, hostString, 4556, builtParameters);
        
        }

        if (commandStr == "get") {
            log->write(Logger::LogLevel::INFO, "Command Processor : building get request");                
            return new CommandTransaction(CommandType::GET, hostString, 4556, builtParameters);
        
        }

        if (commandStr == "list") {
            log->write(Logger::LogLevel::INFO, "Command Processor : building list request");                
            return new CommandTransaction(CommandType::LIST, hostString, 4556, builtParameters);
        }

    } catch (...) {
        log->write(Logger::LogLevel::INFO, "Command Processor : malformed client request. returning null");
        return NULL; 
    }

    //return null if transaction is invalid.
    log->write(Logger::LogLevel::INFO, "Command Processor : malformed client request. returning null");
    return NULL;
}

CommandTransaction* CommandProcessor::buildInfoTransactionResponse(IPaddress destIp, int statusCode, std::string message, bool isSuccess) {

    //TODO : Currently ports are hard coded because client sends on a random port #.

    const char *host = SDLNet_ResolveIP(&destIp);
    Uint32 ipNum = SDL_SwapBE32(destIp.host);
    Uint16 port = 4556; //SDL_SwapBE16(destIp.port);
           
    std::string hostString(host);     

    if (hostString.length() > 0 && port > 0) {

        std::unordered_map<std::string, std::string> params;
        if (isSuccess) {
            params.insert({"status", "success"});
        } else {
            params.insert({"status", "failure"});
        }

        params.insert({"statusCode", std::to_string(statusCode)});

        if (message.length() > 0) {
            params.insert({"message", message});
        }

        return new CommandTransaction(
            CommandType::INFO,
            hostString,
            port,
            params
        );
    } 

    log->write(Logger::LogLevel::INFO, "Command Processor : failed to build info resposne. returning null");
    return NULL;
}

std::unordered_map<std::string, std::string> CommandProcessor::buildParameters(std::string rawParamString) {

    std::unordered_map<std::string, std::string> resultParams;

    //only parse params between param delimiters. Anything past final param end delimiter
    //will be ignored.    
    std::size_t startLoc = rawParamString.find(CommandConstants::PARAMETERS_START_DELIMITER);
    std::size_t endLoc = rawParamString.find(CommandConstants::PARAMETERS_END_DELIMITER); 
    std::string parameters = rawParamString.substr(startLoc + 2, endLoc - startLoc);

    log->write(Logger::LogLevel::INFO, "Command Processor : Unprocessed Params: " + parameters);

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
        log->write(Logger::LogLevel::DEBUG, "Command Processor : result key: " 
            + p.first + " result value: " + p.second);
    }

    return resultParams;

}


CommandTransaction* CommandProcessor::processAddCommand(CommandTransaction *cmd) {
    log->write(Logger::LogLevel::INFO, "Command Processor : Adding new user to game.");    

    //make sure it's actually an add command.
    if (cmd->getCommandType() == CommandType::ADD) {
        try {
            std::string username = cmd->getParameters().at("user");        
            User *newUser = new User(username);

            //add user to the game and register them.
            gameState.addUser(newUser);
            gameState.addRegistration(Registration(username, cmd->getHost(), cmd->getPort()));

            log->write(Logger::LogLevel::INFO, "Command Processor : Added user " + username + " to game.");

            std::unordered_map<std::string, std::string> params;
            params.insert({"status", "success"});
            return new CommandTransaction(
                CommandType::INFO,
                cmd->getHost(),
                cmd->getPort(),
                params
            );
            
        } catch (...) {
            log->write(Logger::LogLevel::INFO, "Command Processor : Exception thrown. Failed to add user to game.");
        }

    } else {
        log->write(Logger::LogLevel::ERROR, "Command Processor : Attempted to pass wrong command type to add command.");
    }

    std::unordered_map<std::string, std::string> params;
    params.insert({"status", "failure"});
    return new CommandTransaction(
        CommandType::INFO,
        cmd->getHost(),
        cmd->getPort(),
        params
    );
}

CommandTransaction* CommandProcessor::processGetCommand(CommandTransaction *cmd) {
    log->write(Logger::LogLevel::INFO, "Command Processor : Get user info.");

    //make sure it's actually anget command.
    if (cmd->getCommandType() == CommandType::GET) {
        try {
            std::string username = cmd->getParameters().at("user");        
            User *foundUser = gameState.getUser(username);
            bool regStatus = gameState.getUserRegistrationStatus(username);
                        
            if (foundUser != NULL) {
                std::unordered_map<std::string, std::string> params;
                params.insert({"status", "success"});
                params.insert({"user", foundUser->getUsername()});
                params.insert({"x", std::to_string(foundUser->getX())});
                params.insert({"y", std::to_string(foundUser->getY())});
                params.insert({"isActive", std::to_string(regStatus)});

                log->write(Logger::LogLevel::INFO, "Command Processor : Found user " + username + " in game.");

                return new CommandTransaction(
                    CommandType::INFO,
                    cmd->getHost(),
                    cmd->getPort(),
                    params
                );    
            } 

            log->write(Logger::LogLevel::INFO, "Command Processor : User " + username + " was not found.");            
        } catch (...) {
            log->write(Logger::LogLevel::INFO, "Command Processor : Exception thrown. Failed to get user info.");
        }

    } else {
        log->write(Logger::LogLevel::ERROR, "Command Processor : Attempted to pass wrong command type to get command.");
    }

    std::unordered_map<std::string, std::string> params;
    params.insert({"status", "failure"});
    return new CommandTransaction(
        CommandType::INFO,
        cmd->getHost(),
        cmd->getPort(),
        params
    );
}

CommandTransaction* CommandProcessor::processListCommand(CommandTransaction *cmd) {
    log->write(Logger::LogLevel::INFO, "Command Processor : list users");

    //make sure it's actually a list command.
    if (cmd->getCommandType() == CommandType::LIST) {
        try {
            
            std::vector<User*> foundUsers = gameState.getUsers();            
            std::unordered_map<std::string, std::string> params;

            for (auto it = foundUsers.begin(); it < foundUsers.end(); ++it) {                                
                std::string username = (*it)->getUsername();    
                bool regStatus = gameState.getUserRegistrationStatus(username);

                params.insert({username + ".user", username});
                params.insert({username + ".x", std::to_string((*it)->getX())});
                params.insert({username + ".y", std::to_string((*it)->getY())});
                params.insert({username + ".isActive", std::to_string(regStatus)});
            }

            params.insert({"status", "success"});
            
            log->write(Logger::LogLevel::INFO, "Command Processor : Found " 
                            + std::to_string(foundUsers.size()) + " users in game.");

            return new CommandTransaction(
                CommandType::INFO,
                cmd->getHost(),
                cmd->getPort(),
                params
            );
            
        } catch (...) {
            log->write(Logger::LogLevel::INFO, "Command Processor : Exception thrown. Failed to get users.");
        }

    } else {
        log->write(Logger::LogLevel::ERROR, "Command Processor : Attempted to pass wrong command type to list command.");
    }

    std::unordered_map<std::string, std::string> params;
    params.insert({"status", "failure"});
    return new CommandTransaction(
        CommandType::INFO,
        cmd->getHost(),
        cmd->getPort(),
        params
    );
}


CommandTransaction* CommandProcessor::processUpdateCommand(CommandTransaction *cmd) {
    log->write(Logger::LogLevel::INFO, "Command Processor : Update user info.");

    //make sure it's actually anget command.
    if (cmd->getCommandType() == CommandType::UPDATE) {
        try {
            std::string username = cmd->getParameters().at("user");              

            User *userUpdate = new User(username);

            //only update values that exist in command.
            if (cmd->getParameters().find("x") !=  cmd->getParameters().end()) {
                int x = atoi(cmd->getParameters().at("x").c_str());
                userUpdate->setX(x);
            }     

            if (cmd->getParameters().find("y") !=  cmd->getParameters().end()) {
                int y = atoi(cmd->getParameters().at("y").c_str());
                userUpdate->setY(y);
            }   
           
            //update user entry in gamestate.
            gameState.updateUser(userUpdate);            
            log->write(Logger::LogLevel::INFO, "Command Processor : Updated user " + username + ".");

            //output params.
            std::unordered_map<std::string, std::string> params;
            params.insert({"status", "success"});

            return new CommandTransaction(
                CommandType::INFO,
                cmd->getHost(),
                cmd->getPort(),
                params
            );                 
        } catch (...) {
            log->write(Logger::LogLevel::INFO, "Command Processor : Exception thrown. Failed to get user info.");
        }

    } else {
        log->write(Logger::LogLevel::ERROR, "Command Processor : Attempted to pass wrong command type to get command.");
    }

    std::unordered_map<std::string, std::string> params;
    params.insert({"status", "failure"});
    return new CommandTransaction(
        CommandType::INFO,
        cmd->getHost(),
        cmd->getPort(),
        params
    );
}