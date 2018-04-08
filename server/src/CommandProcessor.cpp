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

    try {
        //build transaction based on request
        log->write(Logger::LogLevel::DEBUG, "Command Processor : Raw Data string: " + dataString);
    
        //verify server key sent is correct
        std::size_t keyPos = dataString.find(serverKey);
        std::string key = dataString.substr(keyPos, serverKey.length());
        
        if (key != serverKey) {
            log->write(Logger::LogLevel::ERROR, "Command Processor : Request server key does not match. Key supplied: " + key);
            return NULL;
        }

        //get command string from request.
        std::size_t cmdStart = dataString.find_last_of('|') + 1;
        std::size_t cmdEnd = dataString.find_first_of('>');
        int cmdLength = cmdEnd - cmdStart;
        std::string commandStr = dataString.substr(cmdStart, cmdLength); 
        
        //get params
        builtParameters = buildParameters(dataString); 

        log->write(Logger::LogLevel::DEBUG, "Command Processor : Data string: " + dataString 
                    + " cmd: " + commandStr);

        //execute command
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
    return NULL;
}

std::unordered_map<std::string, std::string> CommandProcessor::buildParameters(std::string rawParamString) {

    std::unordered_map<std::string, std::string> resultParams;

    //only parse params between param delimiters. Anything past final param end delimiter
    //will be ignored.
    std::size_t startLoc = rawParamString.find("[{");
    std::size_t endLoc = rawParamString.find("]"); 
    std::string parameters = rawParamString.substr(startLoc + 2, endLoc - startLoc);

    log->write(Logger::LogLevel::INFO, "Command Processor : Unprocessed Params: " + parameters);

    //iterate through and get each key value pair
    size_t paramPos = 0;
    std::string paramKeyValuePairString;
    while ((paramPos = parameters.find("}")) != std::string::npos) {
        paramKeyValuePairString = parameters.substr(0, paramPos);                
        parameters.erase(0, paramPos + 2);

        //found params, add to param map.
        std::string key = paramKeyValuePairString.substr(0, paramKeyValuePairString.find(":"));
        std::string value = paramKeyValuePairString.substr(
            paramKeyValuePairString.find(":") + 1,
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