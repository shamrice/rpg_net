#ifndef __COMMAND_PROCESSOR__
#define __COMMAND_PROCESSOR__

#include <SDL/SDL_net.h>
#include "CommandType.h"
#include "CommandTransaction.h"
#include "Logger.h"
#include "GameState.h"

class CommandProcessor {

    public:
        CommandProcessor(std::string serverKey);
        CommandTransaction* executeCommand(CommandTransaction *request);
        CommandTransaction* buildTransaction(IPaddress ip, const char *data);

    private: 
        CommandTransaction* processAddCommand(CommandTransaction *cmd);
        CommandTransaction* processGetCommand(CommandTransaction *cmd);
        CommandTransaction* processListCommand(CommandTransaction *cmd);
        GameState gameState;       
        Logger *log;
        std::string serverKey;
};

#endif