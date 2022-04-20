#include <stdlib.h>
#include <Arduino.h>

class SwitchCo {
    private:
        //module specific
        byte canID;
        String friendly_name;
        String hw_version;
        byte data_buffer[8];
        //output related
        boolean *digitalIO; // which outputs are Digital IO (1) or analog IO (0)
        const int pwm_freq=10000;
        const int pwm_res=8; //resolution of pwm outputs
    public:
        
        SwitchCo(byte canID,String friendly_name,boolean *digitialIO);
        void init();
        void setup_outputs();


};