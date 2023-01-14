#include "switchCo.h"


SwitchCo::SwitchCo(byte canID, String friendly_name,boolean* digitialIOarr):
    canID(canID),
    friendly_name(friendly_name),
    digitalIO(digitialIOarr)
{

    setup_inputs();
    setup_outputs();
    //initialize data to 0
    for(int i=0;i<8;i++){
        this->data_buffer[i]=0x00;
    }
    this->heartbeat_interval=2000;
    this->long_press_val=500;
    this->double_press_val=300;
    this->now=millis();
    //this->can_controller=GCANController(this->canID);
}

//Define input pins
void SwitchCo::setup_inputs(){
    for(int i=0;i<7;i++){
        pinMode(in_gpio[i], INPUT_PULLUP);
    }
}
//----------Tools-----------------//


void SwitchCo::long_to_data_buffer(long input){
  
  for (int i = 0; i < 4; i++)
  {
    data_buffer[i] = ((input >> (8 * i)) & 0XFF);
  }
}

void SwitchCo::setup_outputs(){
    //Gpio pins connected to outputs L1->L6
    for(int i=0;i<7;i++){
        if(this->digitalIO[i]){
            Serial.println("Dit is digital");
            pinMode(out_gpio[i], OUTPUT);
        }
        else{
            // configure LED PWM functionalitites
            ledcSetup(0,this->pwm_freq, this->pwm_res);
            ledcAttachPin(out_gpio[i], 0);
        }
        this->last_press[i]=0;
        this->last_release[i]=0; 
        this->hold_time[i]=0;
        
        this->input_state[i]=0;
        this->last_input_state[i]=0;
        this->hold_sent[i]=0;
        this->multiple_press[i]=0;
    }
}

//-----------Button Logic------------//
void SwitchCo::press_react(int index){
  last_press[index]=millis();
}
void SwitchCo::release_react(int index){
  now=millis();
  hold_time[index]=now-last_press[index];
  //check for double press
  if(now-last_release[index]<double_press_val && multiple_press[index]){
    //double press detected
    Serial.println("double press detected");
    this->can_controller.send_can_msg(this->can_controller.give_can_id(0x00,0x01,0x01),data_buffer,1);
    multiple_press[index]=0;
  }
  else{
    multiple_press[index]=1;
  }
  last_release[index]=now;
  hold_sent[index]=0;
  
}
void SwitchCo::hold_react(){
  Serial.println("Button is hold for longer");
  this->can_controller.send_can_msg(this->can_controller.give_can_id(0x00,0x01,0x02),data_buffer,1);  
}

// Output logic
void SwitchCo::set_output(int index, int duty,boolean state){
    //check wether its digital out or PWM
    if(this->digitalIO[index]){
        digitalWrite(out_gpio[index],state);
    }
    else{
        if(state){
            ledcWrite(out_gpio[index], duty);
        }
        else{
            ledcWrite(out_gpio[index], 0);
        }
    }
}



void SwitchCo::loop(){
    //check timers
    
    //read inputs
    for(int i=0;i<7;i++){
        int in_select=this->in_gpio[i];
        //switchco inputs are pullup
        input_state[i]=!digitalRead(in_select);
        if(input_state[i]!=last_input_state[i]){
            //Debounce input
            if(millis()-last_press[i] <20){
                return;
            }
            if(input_state[i]){
            Serial.println("going from 0 --> 1");
            //going from 0 --> 1
            press_react(i);
            }
            if(last_input_state[i]){
            Serial.println("going from 1 --> 0");
            //going from 1 --> 0
            release_react(i); 
            }
            last_input_state[i]=input_state[i];
        }
        else if(input_state[i]&!hold_sent[i]){
        //button is still pressed
        if(millis()-last_press[i]>long_press_val){
        //button pressed longer than 500ms
        hold_react();
        hold_sent[i]=1;
        }
    }
    if(millis()-last_release[i]>double_press_val && multiple_press[i]){
            if(hold_time[i]<500){
                //just a single click happend send 
                Serial.print("single click detected hold time: ");Serial.println(hold_time[i]);
                this->can_controller.send_can_msg(this->can_controller.give_can_id(0x00,0x01,0x00),data_buffer,1);
            }
            else{
                //Long release detected 
                Serial.print("Long release  detected hold time: ");Serial.println(hold_time[i]);
                long_to_data_buffer(hold_time[i]);
                this->can_controller.send_can_msg(this->can_controller.give_can_id(0x00,0x01,0x03),data_buffer,4);
            }
            multiple_press[i]=0;
        }
    }

}