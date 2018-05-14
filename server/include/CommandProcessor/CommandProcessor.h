#ifndef __COMMAND_PROCESSOR__
#define __COMMAND_PROCESSOR__

#include <SDL/SDL_net.h>
#include <stdexcept>
#include "Logger.h"
#include "GameState.h"
#include "CommandProcessor/CommandType.h"
#include "CommandProcessor/CommandTransaction.h"
#include "CommandProcessor/CommandConstants.h"
#include "CommandProcessor/ResponseConstants.h"
#include "CommandProcessor/TransactionBuilder.h"

class CommandProcessor {

    /*
    * TODO: 
    *   This needs to be refactored. Is potentially becoming a "god" class for all 
    *   requests. Mutliple ideas:
    * 
    *   At very least, all these build methods should be moved into some sort of 
    *   CommandTransactionBuilder class instead of being part of the processing class.
    * 
    *   All the separate private process command methods should be moved into their
    *   own virtual classes inheriting a base class.
    * 
    *   Would like to use syntax something like:
    *       AddCommandProcessor addCmd = CommandProcessor.get(COMMANDTYPE.ADD); 
    * 
    *   Or even better:
    *       CommandTransaction addReqTrans = CommandTransactionBuilder.build(..params...);
    *       CommandProcessor<AddCommand> addProcessor(serverKey, addReqTrans);
    *       CommandTransaction result = addProcessor.executeCommand();
    *
    */


    public:
        CommandTransaction* executeCommand(CommandTransaction *request);

    private: 
        CommandTransaction* processAddCommand(CommandTransaction *cmd);
        CommandTransaction* processGetCommand(CommandTransaction *cmd);
        CommandTransaction* processListCommand(CommandTransaction *cmd);
        CommandTransaction* processUpdateCommand(CommandTransaction *cmd);
        CommandTransaction* processNotificationCommand(CommandTransaction *cmd);

        TransactionBuilder transactionBuilder;                                   
};

#endif
