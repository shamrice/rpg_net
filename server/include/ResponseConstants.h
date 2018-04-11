#ifndef __RESPONSE_CONSTANTS__
#define __RESPONSE_CONSTANTS__

#include <string>

namespace ResponseConstants {

    //Response codes
    const int BAD_REQUEST_CODE = 400;
    const int NOT_ACTIVE_CODE = 9000;

    //response messages
    const std::string BAD_REQUEST_MSG = "BAD_REQUEST";
    const std::string NOT_ACTIVE_MSG = "NOT_ACTIVE";

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