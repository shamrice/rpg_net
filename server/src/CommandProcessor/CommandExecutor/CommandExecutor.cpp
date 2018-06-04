
#include "CommandProcessor/CommandExecutor/CommandExecutor.h"

 CommandExecutor::CommandExecutor(CommandTransaction *cmd) {
     this->cmd = cmd;
 }

 CommandTransaction* CommandExecutor::execute() {

    CommandTransaction *response = NULL;

    if (cmd != NULL) {

        switch (cmd->getCommandAction()) {
            case CommandAction::ADD: 
                response = executeAdd();
                break;
            case CommandAction::GET:
                response = executeGet();
                break;
            case CommandAction::LIST:
                response = executeList();
                break;
            case CommandAction::UPDATE:
                response = executeUpdate();
                break;
            case CommandAction::NOTIFICATION:
                response = executeNotification();
                break;
            default:
                Logger::write(Logger::INFO, "CommandExecutor : No method to execute command type.");
                break;
        }

    } else {
        Logger::write(Logger::ERROR, "CommandExecutor : Command was null. Nothing to do.");
    }

    return response;
}