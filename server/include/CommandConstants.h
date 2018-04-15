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

}

#endif