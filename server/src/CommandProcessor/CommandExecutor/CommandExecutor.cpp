
#include "CommandProcessor/CommandExecutor/CommandExecutor.h"

 CommandExecutor::CommandExecutor(CommandTransaction *cmd) {
     this->cmd = cmd;
 }

 CommandTransaction* CommandExecutor::execute() {

    CommandTransaction *response = NULL;

    if (cmd != NULL) {

        switch (cmd->getCommandType()) {
            case CommandType::ADD: 
                response = executeAdd();
                break;
            case CommandType::GET:
                response = executeGet();
                break;
            case CommandType::LIST:
                response = executeList();
                break;
            case CommandType::UPDATE:
                response = executeUpdate();
                break;
            case CommandType::NOTIFICATION:
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