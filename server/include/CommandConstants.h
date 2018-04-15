#ifndef __COMMAND_CONSTANTS__
#define __COMMAND_CONSTANTS__

#include <string>

namespace CommandConstants {

    const char SERVER_KEY_DELIMITER = '|';
    const char COMMAND_DELIMITER = '>';
    const std::string PARAMETERS_START_DELIMITER = "[{";
    const std::string PARAMETERS_END_DELIMITER = "]";
    const std::string PARAMETER_END_DELIMITER = "}";
    const std::string PARAMETER_KEY_DELIMITER = ":";

    const std::string UPDATE_COMMAND = "upd";
    const std::string ADD_COMMAND = "add";
    const std::string GET_COMMAND = "get";
    const std::string LIST_COMMAND = "list";
    const std::string INFO_COMMAND = "info";
    const std::string NOTIFICATION_COMMAND = "talk";

    //params
    const std::string STATUS_KEY = "status";
    const std::string STATUS_SUCCESS = "success";
    const std::string STATUS_FAILURE = "failure";

    const std::string STATUS_CODE_KEY = "statusCode";
    const std::string MESSAGE_KEY = "message";

    const std::string NOTIFICATION_TO_KEY = "to";
    const std::string NOTIFICATION_FROM_KEY = "from";
    const std::string NOTIFICATION_MESSAGE_KEY = "chatmsg";

    const std::string USER_KEY = "user";
    const std::string IS_ACTIVE_KEY = "isActive";
    const std::string X_KEY = "x";
    const std::string Y_KEY = "y";
}

#endif