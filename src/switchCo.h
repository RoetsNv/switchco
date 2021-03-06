#include <stdlib.h>
#include <Arduino.h>

class SwitchCo {
    private:
        //module specific
        byte canID;
        String friendly_name;
        String hw_version;
        byte data_buffer[8];
        unsigned long now;
        unsigned long heartbeat_interval; //when heartbeats should be send
        unsigned long long_press_val; //max hold time until it is not interpreted as a click but a long press
        unsigned long double_press_val; //max time between 2 clicks to be a double press
        //output related
        const int out_gpio[7]={23,25,14,12,19,18,17};
        boolean *digitalIO; // which outputs are Digital IO (1) or analog IO (0)
        const int pwm_freq=10000;
        const int pwm_res=8; //resolution of pwm outputs
        boolean output_state[7];
        int output_[7];

        //input related
        const int in_gpio[7]={15,33,26,27,13,4,16};
        unsigned long last_press[7];//last press of 6 inputs
        unsigned long last_release[7];
        unsigned long hold_time[7];
        boolean input_state[7];
        boolean last_input_state[7];
        boolean hold_sent[7 ];
        boolean multiple_press[7];



    public:
        
        SwitchCo(byte canID,String friendly_name,boolean *digitialIO);
        void init();
        void setup_outputs();
        void press_react(int index);
        void loop();
        void release_react(int index);
        void set_output(int index, int duty,boolean state);

};