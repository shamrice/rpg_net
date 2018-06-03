#ifndef __NOTIFICATION_COMMAND_EXECUTOR_H__
#define __NOTIFICATION_COMMAND_EXECUTOR_H__

#include "CommandProcessor/CommandExecutor/CommandExecutor.h"

class NotificationCommandExecutor : public CommandExecutor {

    public:
        NotificationCommandExecutor(CommandTransaction *cmd) : CommandExecutor(cmd) { };

    private:

        //base class overrides
        CommandTransaction* executeAdd();
        CommandTransaction* executeGet();
        CommandTransaction* executeList();
        CommandTransaction* executeUpdate();
        CommandTransaction* executeNotification(); 

};

#endif