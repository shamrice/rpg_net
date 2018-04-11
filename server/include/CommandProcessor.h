#ifndef __COMMAND_PROCESSOR__
#define __COMMAND_PROCESSOR__

#include <SDL/SDL_net.h>
#include "CommandType.h"
#include "CommandTransaction.h"
#include "Logger.h"
#include "GameState.h"
#include "CommandConstants.h"
#include "ResponseConstants.h"

class CommandProcessor {

    public:
        CommandProcessor(std::string serverKey);
        CommandTransaction* executeCommand(CommandTransaction *request);
        CommandTransaction* buildTransaction(IPaddress ip, const char *data);
        CommandTransaction* buildInfoTransactionResponse(IPaddress destIp, int statusCode, std::string message, bool isSuccess);
        CommandTransaction* buildInfoTransactionResponse(std::string host, int port, int statusCode, std::string message, bool isSuccess);
        
    private: 
        CommandTransaction* processAddCommand(CommandTransaction *cmd);
        CommandTransaction* processGetCommand(CommandTransaction *cmd);
        CommandTransaction* processListCommand(CommandTransaction *cmd);
        CommandTransaction* processUpdateCommand(CommandTransaction *cmd);
        std::unordered_map<std::string, std::string> buildParameters(std::string rawParamString);
        
        GameState gameState;               
        std::string serverKey;        
};

#endif