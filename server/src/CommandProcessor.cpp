#include "CommandProcessor.h"

CommandProcessor::CommandProcessor() { }

CommandTransaction* CommandProcessor::executeCommand(CommandTransaction *request) {
    //debug bs right now

    if (request->getCommandType() == CommandType::UPDATE) {
        std::unordered_map<std::string, std::string> respData;
        respData.insert({"p1.x", "45"});
        respData.insert({"p1.y", "25"});
        respData.insert({"p1.hp", "150"});

        return new CommandTransaction(
            request->getCommandType(), 
            request->getHost(),
            request->getPort(),
            respData
        );
    }

    //return null when response not needed.
    return NULL;
}

CommandTransaction* CommandProcessor::buildTransaction(IPaddress ip, const char *data) {
     
    const char *host = SDLNet_ResolveIP(&ip);
    Uint32 ipNum = SDL_SwapBE32(ip.host);
    Uint16 port = SDL_SwapBE16(ip.port);
                
    std::string dataString(data);  
    std::string hostString(host);     

    std::unordered_map<std::string, std::string> builtParameters;              

    //build transaction based on request

    if (dataString == "exit" || dataString == "quit") {
        return new CommandTransaction(CommandType::SHUTDOWN, hostString, port, builtParameters);
    }

    if (dataString == "update") {
        builtParameters.insert({dataString, dataString});
        return new CommandTransaction(CommandType::UPDATE, hostString, 4556, builtParameters);
    }

    //return null if transaction is invalid.
    return NULL;
}

