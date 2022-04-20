#include "switchCo.h"


SwitchCo::SwitchCo(byte canID, String friendly_name,boolean *digitialIO):
    canID(canID),
    friendly_name(friendly_name),
    digitalIO(digitalIO)
{
    setup_inputs();

}
//Define input pins
void setup_inputs(){
    //Built in switch
    pinMode(15, INPUT);
    //Switch inputs S1-S6
    pinMode(33, INPUT);
    pinMode(26, INPUT);
    pinMode(27, INPUT);
    pinMode(13, INPUT);
    pinMode(4, INPUT);
    pinMode(16, INPUT);
}

void SwitchCo::setup_outputs(){
    //Gpio pins connected to outputs L1->L6
    int gpio[6]={25,14,12,19,18,17};
    for(int i=0;i<6;i++){
        if(this->digitalIO[i]){
           pinMode(gpio[i], OUTPUT);
        }
        else{
            // configure LED PWM functionalitites
            ledcSetup(0,this->pwm_freq, this->pwm_res);
            ledcAttachPin(gpio[i], 0);
        }  
    }
}