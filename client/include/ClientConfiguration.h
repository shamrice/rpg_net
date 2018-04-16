#ifndef __CLIENT_CONFIGURATION__
#define __CLIENT_CONFIGURATION__

#include <string>

class ClientConfiguration {

    public: 
        ClientConfiguration(std::string serverHost, int serverPort, int clientPort);
        bool configure();
        std::string getServerHost();
        int getServerPort();
        int getClientPort();

    private:
        std::string serverHost;
        int serverPort;
        int clientPort;

};

#endif