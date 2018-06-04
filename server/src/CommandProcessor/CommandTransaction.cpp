#include "CommandProcessor/CommandTransaction.h"

CommandTransaction::CommandTransaction(CommandType type, CommandAction action, std::string hostname, 
                                        int portNum, std::unordered_map<std::string, std::string> params) {
    commandType = type;
    commandAction = action;
    host = hostname;
    port = portNum;
    parameters = params;
}

CommandType CommandTransaction::getCommandType() {
    return commandType;
}

CommandAction CommandTransaction::getCommandAction() {
    return commandAction;
}

std::string CommandTransaction::getHost() {
    return host;
}
    
int CommandTransaction::getPort() {
    return port;
}

std::string CommandTransaction::getFormattedResponse() {

    std::string respString = "";

    respString += getCommandTypeString() + CommandConstants::COMMAND_SPLIT_CHAR
                    + getCommandActionString() + "<[";

    for (auto it = parameters.begin(); it != parameters.end(); ++it) {
        respString += "{" + it->first + ":" + it->second + "}";
    }

    respString += "]";    

    return respString;
}

std::unordered_map<std::string, std::string> CommandTransaction::getParameters() {
    return parameters;
}

std::string CommandTransaction::getCommandActionString() {
    
    std::string response = "";

    switch(commandAction) {
        case CommandAction::UPDATE:
            response = CommandConstants::UPDATE_COMMAND;
            break;
        
        case CommandAction::ADD:
            response = CommandConstants::ADD_COMMAND;
            break;

        case CommandAction::GET:
            response = CommandConstants::GET_COMMAND;
            break;

        case CommandAction::INFO:
            response = CommandConstants::INFO_COMMAND;
            break;

        case CommandAction::LIST:
            response = CommandConstants::LIST_COMMAND;
            break;
    }

    return response;
}

std::string CommandTransaction::getCommandTypeString() {
    
    std::string response = "";

    switch(commandType) {
        case CommandType::SYSTEM:
            response = CommandConstants::SYSTEM_COMMAND_TYPE;
            break;
        
        case CommandType::USER:
            response = CommandConstants::USER_COMMAND_TYPE;
            break;

        case CommandType::NOTIFICATION:
            response = CommandConstants::NOTIFICATION_COMMAND_TYPE;
            break;
    }

    return response;
}



