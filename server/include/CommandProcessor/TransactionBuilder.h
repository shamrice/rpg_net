#ifndef __TRANSACTION_BUILDER_H__
#define __TRANSACTION_BUILDER_H__

#include <string>
#include <unordered_map>
#include <SDL/SDL_net.h>
#include "Logger.h"
#include "CommandTransaction.h"
#include "Core/GameState.h"
#include "Core/Registration.h"

class TransactionBuilder {

    public:
        CommandTransaction* buildRequest(IPaddress ip, const char *data);
        CommandTransaction* buildResponse(IPaddress destIp, int statusCode, std::string message, bool isSuccess);
        CommandTransaction* buildResponse(std::string host, int port, int statusCode, std::string message, bool isSuccess);
        CommandTransaction* buildResponse(std::string host, int port, int statusCode, std::string message, bool isSuccess, std::unordered_map<std::string, std::string> params); 
    
    private:
        std::unordered_map<std::string, std::string> buildParameters(std::string rawParamString);

};

#endif