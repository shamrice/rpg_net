#include "CommandProcessor.h"

CommandProcessor::CommandProcessor() { }

CommandTransaction CommandProcessor::executeCommand(CommandTransaction request) {
    //debug bs right now

    std::unordered_map<std::string, std::string> respData;
    respData.insert({"p1.x", "45"});
    respData.insert({"p1.y", "25"});
    respData.insert({"p1.hp", "150"});

    CommandTransaction response(
        request.getCommandType(), 
        request.getHost(),
        request.getPort(),
        respData
    );

    return response;   
}

