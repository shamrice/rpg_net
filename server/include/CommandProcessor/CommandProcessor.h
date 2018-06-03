#ifndef __COMMAND_PROCESSOR__
#define __COMMAND_PROCESSOR__

#include <SDL/SDL_net.h>
#include "Logger.h"
#include "CommandProcessor/CommandType.h"
#include "CommandProcessor/CommandExecutor/CommandExecutor.h"
#include "CommandProcessor/CommandExecutor/UserCommandExecutor.h"
#include "CommandProcessor/CommandExecutor/NotificationCommandExecutor.h"

class CommandProcessor {

    public:
        CommandTransaction* executeCommand(CommandTransaction *request);
                        
};

#endif
