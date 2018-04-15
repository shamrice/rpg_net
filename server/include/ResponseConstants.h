#ifndef __RESPONSE_CONSTANTS__
#define __RESPONSE_CONSTANTS__

#include <string>

namespace ResponseConstants {

    //Response codes
    const int BAD_REQUEST_CODE = 400;
    const int NOT_ACTIVE_CODE = 9000;
    const int NOT_FOUND_CODE = 9100;
    const int INVALID_CODE = 8000;
    const int PROCESS_FAILURE_CODE = 7000;
    const int SUCCESS_CODE = 1000;

    //response messages
    const std::string BAD_REQUEST_MSG = "BAD_REQUEST";
    const std::string NOT_ACTIVE_MSG = "NOT_ACTIVE";
    const std::string NOT_FOUND_MSG = "USER_NOT_FOUND";
    const std::string INVALID_MSG = "INVALID_CMD";
    const std::string PROCESS_FAILURE_MSG = "CMD_PROCESS_FAILURE";
    const std::string ADD_USER_SUCCESS_MSG = "USER_ADDED";
    const std::string GET_USER_SUCCESS_MSG = "USER_RETREIVED";
    const std::string LIST_USER_SUCCESS_MSG = "USERS_LISTED";
    const std::string UPDATE_USER_SUCCESS_MSG = "USER_UPDATED";
    const std::string NOTIFICATION_SUCCESS_MSG = "NOTIFICATION_QUEUED";
    const std::string SUCCESS_MSG = "SUCCESS";

    //response params
    const std::string STATUS_KEY = "status";
    const std::string STATUS_SUCCESS = "success";
    const std::string STATUS_FAILURE = "failure";

    const std::string STATUS_CODE_KEY = "statusCode";
    const std::string MESSAGE_KEY = "message";

    const std::string USER_KEY = "user";
    const std::string IS_ACTIVE_KEY = "isActive";
    const std::string X_KEY = "x";
    const std::string Y_KEY = "y";

}

#endif