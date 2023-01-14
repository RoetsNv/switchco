#include "switchCo.h"
//globals
boolean fade_up[7]={true,true,true,true,true,true,true};

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
    ledcSetup(0,this->pwm_freq, this->pwm_res);
    for(int i=0;i<7;i++){
        if(this->digitalIO[i]){
            Serial.println("Dit is digital");
            pinMode(out_gpio[i], OUTPUT);
        }
        else{
            // configure LED PWM functionalitites
            ledcSetup(i,this->pwm_freq, this->pwm_res);
            ledcAttachPin(out_gpio[i], i);
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
        this->output_state[index]= int(state);
    }
    else{
        if(state){
            ledcWrite(index, duty);
            this->output_state[index]= int(duty);
        }
        else{
            ledcWrite(index, 0);
            this->output_state[index]= 0;

        }
    }
}

//timers
void on_timer_0(SwitchCo* s){
    if(fade_up[1]){
        int to_set=s->output_state[1]+10;
        if(to_set>254){
            fade_up[1]=false;
            to_set=254;
        }
        s->set_output(1,to_set,true); 
         
    }
    else{
        int to_set=s->output_state[1]-10;
        if(to_set<0){
            fade_up[1]=true;
            to_set=0;
        }  
        //Serial.print("Setting minus: ");Serial.println(to_set);
        s->set_output(1,to_set,true); 
    }
}
void on_timer_1(SwitchCo* s){
    
}

void SwitchCo::on_timer(int index){
    switch(index) {
    case 0:
        on_timer_0(this);
        break;
    case 1:
        on_timer_1(this);
        break;
    default:
        // code block
        Serial.println("ERR_timer");
}
}

void SwitchCo::loop(){
    //check timers
    int index=0;
    for (int req_time : this->timers) {
        if(millis()-this->timer_ticks[index] >=req_time){
            on_timer(index);
            this->timer_ticks[index]=millis();
        }
        index++;
    }
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




