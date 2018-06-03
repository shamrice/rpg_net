#include "CommandProcessor/CommandExecutor/NotificationCommandExecutor.h"

CommandTransaction* NotificationCommandExecutor::executeNotification() {
    if (cmd == NULL) {
        Logger::write(Logger::LogLevel::ERROR, "NotificationCommandExecutor : executeNotification received NULL cmd.");
        return NULL;
    }

    if (cmd->getCommandType() == CommandType::NOTIFICATION) {

        try {
            std::string to = cmd->getParameters().at(CommandConstants::NOTIFICATION_TO_KEY);
            std::string from = cmd->getParameters().at(CommandConstants::USER_KEY);
            std::string message = cmd->getParameters().at(CommandConstants::NOTIFICATION_MESSAGE_KEY);     
        
            Registration *toReg = GameState::getInstance().get<Registration>(to);

            //update chat senders last activity time.
            GameState::getInstance().get<Registration>(from)->updateLastActive();
            
            //make sure to only send message if user exists in registry.
            if (toReg != NULL) {

                Notification noteToSend(from, toReg->getUsername(), message);
                GameState::getInstance().add<Notification>(noteToSend);
             
                Logger::write(Logger::LogLevel::INFO, "NotificationCommandExecutor : Adding notification from " 
                    + from + " to " + to + " with message " + message);

                return transactionBuilder.buildResponse(    
                    cmd->getHost(),
                    cmd->getPort(),
                    ResponseConstants::SUCCESS_CODE,
                    ResponseConstants::NOTIFICATION_SUCCESS_MSG,
                    true
                ); 
            }
        } catch (std::out_of_range outOfRangeEx) {
            Logger::write(Logger::LogLevel::ERROR, "NotificationCommandExecutor : Exception thrown. Failed to execute notification command "); 
            std::cerr << "NotificationCommandExecutor : executeNotification : Exception = " << outOfRangeEx.what() << std::endl;
            
            return transactionBuilder.buildResponse(
                cmd->getHost(),
                cmd->getPort(),
                ResponseConstants::PROCESS_FAILURE_CODE,
                ResponseConstants::PROCESS_FAILURE_MSG,
                false
            );
        }
    } 

    return transactionBuilder.buildResponse(
        cmd->getHost(),
        cmd->getPort(),
        ResponseConstants::INVALID_CODE,
        ResponseConstants::INVALID_MSG,
        false
    );
}

CommandTransaction* NotificationCommandExecutor::executeAdd() {
    Logger::write(Logger::ERROR, "NotificationCommandExecutor : Add command not implemented.");
    return NULL;
}

CommandTransaction* NotificationCommandExecutor::executeGet() {
    Logger::write(Logger::ERROR, "NotificationCommandExecutor : Get command not implemented.");
    return NULL;
}

CommandTransaction* NotificationCommandExecutor::executeList() {
    Logger::write(Logger::ERROR, "NotificationCommandExecutor : List command not implemented.");
    return NULL;
}

CommandTransaction* NotificationCommandExecutor::executeUpdate() {
    Logger::write(Logger::ERROR, "NotificationCommandExecutor : Update command not implemented.");
    return NULL;
}

    