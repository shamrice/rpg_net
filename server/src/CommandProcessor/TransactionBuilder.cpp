#include "CommandProcessor/TransactionBuilder.h"

CommandTransaction* TransactionBuilder::buildRequest(IPaddress ip, const char *data) {
     
    const char *host = SDLNet_ResolveIP(&ip);
    Uint32 ipNum = SDL_SwapBE32(ip.host);
    Uint16 port = SDL_SwapBE16(ip.port);
                
    std::string dataString(data);  
    std::string hostString(host);     

    std::unordered_map<std::string, std::string> builtParameters;              

    try {
        //build transaction based on request
        Logger::write(Logger::LogLevel::DEBUG, "TransactionBuilder : Raw Data string: " + dataString);
    
        //verify server key sent is correct
        std::string key = "";
        std::string serverKey = GameState::getInstance().getServerKey();
        std::size_t keyPos = dataString.find(serverKey);

        if (keyPos != std::string::npos) {            
            key = dataString.substr(keyPos, serverKey.length());
            Logger::write(Logger::LogLevel::DEBUG, "TransactionBuilder : server key=" + key + " keyPos=" + std::to_string(keyPos));
        }        
        if (key != serverKey) {
            Logger::write(Logger::LogLevel::ERROR, "TransactionBuilder : Request server key does not match. Key supplied: " 
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

        Logger::write(Logger::LogLevel::DEBUG, "TransactionBuilder : Data string: " + dataString 
                    + " cmd: " + commandStr);

        //get user's listening port from registration if not an add command.
        //int port = -1;
        if (commandStr != CommandConstants::ADD_COMMAND) {
            std::string username = builtParameters.at(CommandConstants::USER_KEY);
            Registration *userReg = GameState::getInstance().get<Registration>(username);
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
            Logger::write(Logger::LogLevel::INFO, "TransactionBuilder : building update request");          
            return new CommandTransaction(CommandType::UPDATE, hostString, port, builtParameters);
        }

        if (commandStr == CommandConstants::ADD_COMMAND) {
            Logger::write(Logger::LogLevel::INFO, "TransactionBuilder : building add request");            
            return new CommandTransaction(CommandType::ADD, hostString, port, builtParameters);
        
        }

        if (commandStr == CommandConstants::GET_COMMAND) {
            Logger::write(Logger::LogLevel::INFO, "TransactionBuilder : building get request");                
            return new CommandTransaction(CommandType::GET, hostString, port, builtParameters);
        
        }

        if (commandStr == CommandConstants::LIST_COMMAND) {
            Logger::write(Logger::LogLevel::INFO, "TransactionBuilder : building list request");                
            return new CommandTransaction(CommandType::LIST, hostString, port, builtParameters);
        }

        if (commandStr == CommandConstants::NOTIFICATION_COMMAND) {
            Logger::write(Logger::LogLevel::INFO, "TransactionBuilder : building notification request");                
            return new CommandTransaction(CommandType::NOTIFICATION, hostString, port, builtParameters);
        }

    } catch (...) {
        Logger::write(Logger::LogLevel::INFO, "TransactionBuilder : malformed client request. returning null");
        return NULL; 
    }

    //return null if transaction is invalid.
    Logger::write(Logger::LogLevel::INFO, "TransactionBuilder : malformed client request. returning null");
    return NULL;
}

CommandTransaction* TransactionBuilder::buildResponse(IPaddress destIp, int statusCode, std::string message, bool isSuccess) {

    //TODO : Currently ports are hard coded because client sends on a random port #.

    const char *host = SDLNet_ResolveIP(&destIp);
    Uint32 ipNum = SDL_SwapBE32(destIp.host);
    Uint16 port = 4556; //SDL_SwapBE16(destIp.port);
           
    std::string hostString(host);     
    std::unordered_map<std::string, std::string> params;

    return buildResponse(hostString, port, statusCode, message, isSuccess, params);

}

//Overload method when no params are passed.
CommandTransaction* TransactionBuilder::buildResponse(std::string host, int port, int statusCode, std::string message, bool isSuccess) {
    std::unordered_map<std::string, std::string> params;
    return buildResponse(host, port, statusCode, message, isSuccess, params);
}

CommandTransaction* TransactionBuilder::buildResponse(std::string host, int port, int statusCode, std::string message, bool isSuccess, std::unordered_map<std::string, std::string> params) {
    
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

    Logger::write(Logger::LogLevel::INFO, "TransactionBuilder : failed to build info response. returning null");
    return NULL;
}

std::unordered_map<std::string, std::string> TransactionBuilder::buildParameters(std::string rawParamString) {

    std::unordered_map<std::string, std::string> resultParams;

    //only parse params between param delimiters. Anything past final param end delimiter
    //will be ignored.    
    std::size_t startLoc = rawParamString.find(CommandConstants::PARAMETERS_START_DELIMITER);
    std::size_t endLoc = rawParamString.find(CommandConstants::PARAMETERS_END_DELIMITER); 
    std::string parameters = rawParamString.substr(startLoc + 2, endLoc - startLoc);

    Logger::write(Logger::LogLevel::INFO, "TransactionBuilder : Unprocessed Params: " + parameters);

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
        Logger::write(Logger::LogLevel::DEBUG, "TransactionBuilder : result key: " 
            + p.first + " result value: " + p.second);
    }

    return resultParams;

}
