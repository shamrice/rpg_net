#ifndef __UDP_CLIENT_SERVICE__
#define __UDP_CLIENT_SERVICE__

#include <string>

class UdpClientService {

    public:
        UdpClientService(std::string serverHost, int serverPort, int clientPort);

    private:
        std::string serverHost;
        int serverPort;
        int clientPort;
};

#endif