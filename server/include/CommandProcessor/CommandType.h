#ifndef __COMMAND_TYPE__
#define __COMMAND_TYPE__

/*
 * Header file for command constants.
 * 
 */

namespace CommandTypes {

    enum CommandType {
        SYSTEM,
        USER,
        NOTIFICATION,
        INVALID
    };

}
#endif