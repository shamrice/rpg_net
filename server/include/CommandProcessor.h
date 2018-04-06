#ifndef __COMMAND_PROCESSOR__
#define __COMMAND_PROCESSOR__

#include <SDL/SDL_net.h>
#include "CommandType.h"
#include "CommandTransaction.h"

class CommandProcessor {

    public:
        CommandProcessor();
        CommandTransaction* executeCommand(CommandTransaction *request);
        CommandTransaction* buildTransaction(IPaddress ip, const char *data);
};

#endif