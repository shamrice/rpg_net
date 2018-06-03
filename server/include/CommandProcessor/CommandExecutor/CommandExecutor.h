#ifndef __COMMAND_EXECUTOR_H__
#define __COMMAND_EXECUTOR_H__

//#include <type_traits>
#include <string>
#include "Logger.h"
#include "GameState.h"
#include "CommandProcessor/CommandType.h"
#include "CommandProcessor/CommandTransaction.h"
#include "CommandProcessor/CommandConstants.h"
#include "CommandProcessor/ResponseConstants.h"
#include "CommandProcessor/TransactionBuilder.h"

//template <class T>
class CommandExecutor {

    public:

        /*

            Leaving comment as it can be used possibly in gamestate refactor?

        template<typename T>
        bool get() {}

        template<typename T> 
        bool update(T item) { }
        */

        CommandExecutor(CommandTransaction *cmd);
        CommandTransaction* execute();

    protected:  
        virtual CommandTransaction* executeAdd() = 0;
        virtual CommandTransaction* executeGet() = 0;
        virtual CommandTransaction* executeList() = 0;
        virtual CommandTransaction* executeUpdate() = 0;
        virtual CommandTransaction* executeNotification() = 0;

        CommandTransaction *cmd;
        TransactionBuilder transactionBuilder;   

};

#endif