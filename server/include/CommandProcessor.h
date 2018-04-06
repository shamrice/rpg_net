#ifndef __COMMAND_PROCESSOR__
#define __COMMAND_PROCESSOR__

#include "CommandType.h"
#include "CommandTransaction.h"

class CommandProcessor {

    public:
        CommandProcessor();
        CommandTransaction executeCommand(CommandTransaction request);
};

#endif