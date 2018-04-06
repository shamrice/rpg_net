#include "CommandProcessor.h"

CommandProcessor::CommandProcessor() { 
    //temp having it's own logger...
    log = new Logger(Logger::LogType::CONSOLE);
}

CommandTransaction* CommandProcessor::executeCommand(CommandTransaction *request) {
    //debug bs right now

    CommandTransaction *response = NULL;

    switch (request->getCommandType()) {
        case UPDATE: {
            std::unordered_map<std::string, std::string> respData;
            respData.insert({"p1.x", "45"});
            respData.insert({"p1.y", "25"});
            respData.insert({"p1.hp", "150"});

            response = new CommandTransaction(
                request->getCommandType(), 
                request->getHost(),
                request->getPort(),
                respData
            );  
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

    //build transaction based on request

    std::string commandStr = dataString.substr(0, dataString.find(">"));
    log->write(Logger::LogLevel::DEBUG, "Data string: " + dataString + " cmd: " + commandStr);

    if (commandStr == "exit" || commandStr == "quit") {
        return new CommandTransaction(CommandType::SHUTDOWN, hostString, port, builtParameters);
    }

    if (commandStr == "upd") {
        builtParameters.insert({dataString, dataString});
        return new CommandTransaction(CommandType::UPDATE, hostString, 4556, builtParameters);
    }

    if (commandStr == "add") {
        log->write(Logger::LogLevel::INFO, "Command Processor : building add request");

        try {
            
            std::size_t startLoc = dataString.find_first_of('{');
            std::size_t endLoc = dataString.find_first_of('}');
        
            std::string usernameField = dataString.substr(startLoc, endLoc);

            log->write(Logger::LogLevel::INFO, "Command Processor : usernameField: " + usernameField);

            std::string usernameKey = usernameField.substr(1, usernameField.find(":") - 1);
            std::string username = usernameField.substr(
                usernameField.find(":") + 1, 
                usernameField.size()
            );  

            username.pop_back(); //remove trailing character

            log->write(Logger::LogLevel::INFO, "Command Processor : building add request " 
                    + usernameKey + " : " + username + " from " + usernameField);

            builtParameters.insert({usernameKey, username});
            return new CommandTransaction(CommandType::ADD, hostString, 4556, builtParameters);
        } catch (...) {
            log->write(Logger::LogLevel::INFO, "Command Processor : malformed add request. returning null");
            return NULL; 
        }
    }

    if (commandStr == "get") {
        log->write(Logger::LogLevel::INFO, "Command Processor : building get request");    
        try {
            
            std::size_t startLoc = dataString.find_first_of('{');
            std::size_t endLoc = dataString.find_first_of('}');
        
            std::string usernameField = dataString.substr(startLoc, endLoc);

            log->write(Logger::LogLevel::INFO, "Command Processor : usernameField: " + usernameField);

            std::string usernameKey = usernameField.substr(1, usernameField.find(":") - 1);
            std::string username = usernameField.substr(
                usernameField.find(":") + 1, 
                usernameField.size()
            );  

            username.pop_back(); //remove trailing character

            log->write(Logger::LogLevel::INFO, "Command Processor : building get request " 
                    + usernameKey + " : " + username + " from " + usernameField);

            builtParameters.insert({usernameKey, username});
            return new CommandTransaction(CommandType::GET, hostString, 4556, builtParameters);
        } catch (...) {
            log->write(Logger::LogLevel::INFO, "Command Processor : malformed add request. returning null");
            return NULL; 
        }
    }

    if (commandStr == "list") {
        log->write(Logger::LogLevel::INFO, "Command Processor : building list request");    
        return new CommandTransaction(CommandType::LIST, hostString, 4556, builtParameters);
    }

    //return null if transaction is invalid.
    return NULL;
}


CommandTransaction* CommandProcessor::processAddCommand(CommandTransaction *cmd) {
    log->write(Logger::LogLevel::INFO, "Command Processor : Adding new user to game.");    

    //make sure it's actually an add command.
    if (cmd->getCommandType() == CommandType::ADD) {
        try {
            std::string username = cmd->getParameters().at("user");        
            User newUser(username);
            gameState.addUser(newUser);

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
            User foundUser = gameState.getUser(username);
            
            std::unordered_map<std::string, std::string> params;
            params.insert({"status", "success"});
            params.insert({"user", foundUser.getUsername()});
            params.insert({"x", std::to_string(foundUser.getX())});
            params.insert({"y", std::to_string(foundUser.getY())});

            log->write(Logger::LogLevel::INFO, "Command Processor : Found user " + username + " in game.");

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

CommandTransaction* CommandProcessor::processListCommand(CommandTransaction *cmd) {
    log->write(Logger::LogLevel::INFO, "Command Processor : list users");

    //make sure it's actually a list command.
    if (cmd->getCommandType() == CommandType::LIST) {
        try {
            
            std::vector<User> foundUsers = gameState.getUsers();            
            std::unordered_map<std::string, std::string> params;

            for (auto it = foundUsers.begin(); it < foundUsers.end(); ++it) {                                
                std::string username = it->getUsername();
                params.insert({username + ".user", username});
                params.insert({username + ".x", std::to_string(it->getX())});
                params.insert({username + ".y", std::to_string(it->getY())});
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