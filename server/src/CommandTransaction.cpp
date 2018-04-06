#include "CommandTransaction.h"

CommandTransaction::CommandTransaction(CommandType type, std::string hostname, 
                                        int portNum, std::unordered_map<std::string, std::string> params) {
    commandType = type;
    host = hostname;
    port = portNum;
    parameters = params;
}

CommandType CommandTransaction::getCommandType() {
    return commandType;
}

std::string CommandTransaction::getHost() {
    return host;
}
    
int CommandTransaction::getPort() {
    return port;
}

std::string CommandTransaction::getFormattedResponse() {

    std::string respString = "";

    respString += getCommandTypeString() + ">";

    for (auto it = parameters.begin(); it != parameters.end(); ++it) {
        respString += "{" + it->first + "=" + it->second + "}";
    }

    return respString;
}

std::unordered_map<std::string, std::string> CommandTransaction::getParameters() {
    return parameters;
}

std::string CommandTransaction::getCommandTypeString() {
    
    std::string response = "";

    switch(commandType) {
        case CommandType::UPDATE:
            response = "UPDATE";
            break;
    }

    return response;
}

