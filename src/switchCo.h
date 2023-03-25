#include <stdlib.h>
#include <Arduino.h>
#include <controllers/ginco_can_controller.h>
#include <controllers/sk.h>
#include <Preferences.h>

class SwitchCo {
    private:
        //module specific
        byte moduleID;
        String friendly_name;
        String hw_version;
        byte data_buffer[8];
        GCANController can_controller;
        const int timers[3]={35,500,10000};
        int timer_ticks[3]={0,0,0};
        unsigned long now;
        unsigned long heartbeat_interval; //when heartbeats should be send
        unsigned long long_press_val; //max hold time until it is not interpreted as a click but a long press
        unsigned long double_press_val; //max time between 2 clicks to be a double press
        //output related
        const uint8_t out_gpio[7]={23,25,14,12,19,18,17};
        boolean *digitalOut; // which outputs are Digital IO (1) or analog IO (0)
        boolean *digitalIn; // is input button (0) or generic in (1)
        const int pwm_freq=10000;
        const int pwm_res=8; //resolution of pwm outputs
        //input related
        const uint8_t in_gpio[7]={15,33,26,27,13,4,16};
        unsigned long last_press[7];//last press of 6 inputs
        unsigned long last_release[7];
        unsigned long hold_time[7];
        boolean input_state[7];
        boolean last_input_state[7];
        boolean hold_sent[7];
        boolean multiple_press[7];
        Preferences flash; // Persistant storage ESP32

        void on_timer(int index);
    public:
        sk pixel;
        int output_state[7];
        SwitchCo(byte moduleID,String friendly_name);
        void init();
        void setup_inputs();
        void setup_outputs();
        void long_to_data_buffer(long input);
        void press_react(int index);
        void hold_react(int index);
        void loop();
        void release_react(int index);
        void set_output(int index, int duty,boolean state);
        void on_can_msg(GCanMessage m);
        void heartbeat();
        void read_settings();
};