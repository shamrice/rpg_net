#include "CommandProcessor/CommandProcessor.h"

CommandTransaction* CommandProcessor::executeCommand(CommandTransaction *request) {

    if (request != NULL) {

        CommandExecutor *commandExecutor = NULL;

        //find appropriate command executor
        switch (request->getCommandType()) {
            case CommandType::USER:
                commandExecutor = new UserCommandExecutor(request);
                break;
            case CommandType::NOTIFICATION:
                commandExecutor = new NotificationCommandExecutor(request);
                break;
        }

        //execute command if found and return
        if (commandExecutor != NULL) {
            return commandExecutor->execute();
        } else {
            Logger::write(Logger::ERROR, "Unable to find suitable command executor for command type: " 
                + request->getCommandType());
        }
    } else {
        Logger::write(Logger::ERROR, "Request was null. No command to process.");
    }

    return NULL;
}
