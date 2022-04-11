#include <stdlib.h>
#include <Arduino.h>

class SwitchCo {
    private:
        byte canID;
        String friendly_name;
        String hw_version;
        byte data_buffer[8];
    public:
        
        SwitchCo(byte canID,String friendly_name);
        void init();


};