#ifndef __COMMAND_TRANSACTION__
#define __COMMAND_TRANSACTION__

#include <string>
#include <unordered_map>
#include "CommandType.h"
#include "CommandProcessor/CommandAction.h"
#include "CommandConstants.h"

using namespace CommandTypes;
using namespace CommandActions;

class CommandTransaction {

    public:
        CommandTransaction(CommandType type, CommandAction action, std::string hostname, int portNum, 
                            std::unordered_map<std::string, std::string> params);
        CommandType getCommandType();
        CommandAction getCommandAction();
        std::string getHost();
        int getPort();
        std::string getFormattedResponse();
        std::unordered_map<std::string, std::string> getParameters();

    private:
        std::string getCommandTypeString();
        std::string getCommandActionString();

        CommandType commandType;
        CommandAction commandAction;
        std::string host;
        int port;
        std::unordered_map<std::string, std::string> parameters;
};

#endif