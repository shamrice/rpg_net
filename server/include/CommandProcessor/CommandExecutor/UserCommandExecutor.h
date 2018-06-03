#ifndef __USER_COMMAND_EXECUTOR_H__
#define __USER_COMMAND_EXECUTOR_H__

#include "CommandProcessor/CommandExecutor/CommandExecutor.h"

class UserCommandExecutor : public CommandExecutor {

    public:
        UserCommandExecutor(CommandTransaction *cmd) : CommandExecutor(cmd) { };        

    private:
        //base class overrides
        CommandTransaction* executeAdd();
        CommandTransaction* executeGet();
        CommandTransaction* executeList();
        CommandTransaction* executeUpdate();
        CommandTransaction* executeNotification();   
};

#endif