#ifndef __SERVICE_STATE__
#define __SERVICE_STATE__

class ServiceState {

    public: 
       ServiceState() { isServiceRunning = false; };
       void setIsRunning(bool state);
       bool isRunning();
       
    private: 
       bool isServiceRunning;   
};

#endif