#include "switchCo.h"
//globals
boolean fade_up[7]={true,true,true,true,true,true,true};
boolean show_click_effect[7]={true,true,true,true,true,true,true}; //show effect when pressed (linked input <---> output)
boolean click_effect_running[7]={false,false,false,false,false,false,false}; 
boolean pulse_effect[7]={false,false,false,false,false,false,false}; //show pulse effect on output x
boolean blink_effect[7]={false,false,false,false,false,false,false}; //show blink effect on output x
void reset_effects(int index){
    pulse_effect[index]=false;
    blink_effect[index]=false;
}
void setup_can_ids(GCANController * ctrl){
    ctrl->add_moduleID(0x01);
    ctrl->add_moduleID(0x02);
    ctrl->add_moduleID(0x03);
}

SwitchCo::SwitchCo(byte moduleID, String friendly_name,boolean* digitialIOarr):
    moduleID(moduleID),
    friendly_name(friendly_name),
    digitalIO(digitialIOarr)
{
    setup_inputs();
    setup_outputs();
    this->pixel=sk();
    this->pixel.begin(32,1);
    //initialize data to 0
    for(int i=0;i<8;i++){
        this->data_buffer[i]=0x00;
    }
    this->heartbeat_interval=2000;
    this->long_press_val=500;
    this->double_press_val=300;
    this->now=millis();
    this->can_controller=GCANController(this->moduleID,&this->pixel);
    setup_can_ids(&this->can_controller);

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
  if(show_click_effect[index]){
      this->set_output(index,256,true);
      click_effect_running[index]=true;
  }
}
void SwitchCo::release_react(int index){
  now=millis();
  hold_time[index]=now-last_press[index];
  //check for double press
  if(now-last_release[index]<double_press_val && multiple_press[index]){
    //double press detected
    //Serial.println("double press detected");
    this->can_controller.send_can_msg(this->can_controller.give_can_id(true,this->moduleID,0x00,index,0x01,false),data_buffer,1);
    multiple_press[index]=0;
  }
  else{
    multiple_press[index]=1;
  }
  last_release[index]=now;
  hold_sent[index]=0;
  
}
void SwitchCo::hold_react(int index){
  //Serial.println("Button is hold for longer");
  this->can_controller.send_can_msg(this->can_controller.give_can_id(true,this->moduleID,0x00,index,0x02,false),data_buffer,1);  
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
    
    for(int i = 0;i<7;i++){
        if(pulse_effect[i] && !click_effect_running[i]){
            if(fade_up[i] ){
                int to_set=s->output_state[i]+10;
                if(to_set>254){
                    fade_up[i]=false;
                    to_set=254;
                }
                s->set_output(i,to_set,true); 
            }
            else{
                int to_set=s->output_state[i]-10;
                if(to_set<0){
                    fade_up[i]=true;
                    to_set=0;
                }  
                s->set_output(i,to_set,true); 
            }
        }

        if(click_effect_running[i]){
            int to_set=s->output_state[i]-10;
            if(to_set<=0){
                click_effect_running[i]=false;
                s->set_output(i,0,false);
            }else{
                s->set_output(i,to_set,true);
            }  
        }
    }
    
}
void on_timer_1(SwitchCo* s){
    s->pixel.clear();
    for(int i = 0;i<7;i++){
        if(blink_effect[i]){
            if(s->output_state[i] > 0){
                s->set_output(i,0,false);
            }
            else{
            s->set_output(i,256,true); 
            }
        }
    }   
}
void on_timer_2(SwitchCo* s){
    //send heartbeat
    s->heartbeat();
}
void SwitchCo::heartbeat(){
    this->can_controller.send_can_msg(this->can_controller.give_can_id(true,this->moduleID,0xFF,0x00,0x00,false),data_buffer,1);
}
void SwitchCo::on_timer(int index){
    switch(index) {
    case 0:
        on_timer_0(this);
        break;
    case 1:
        on_timer_1(this);
        break;
    case 2:
        on_timer_2(this);
    break;
    default:
        // code block
        Serial.println("ERR_timer");
}
}

void  SwitchCo::on_can_msg(GCanMessage m){
    if(!m.event && (m.source_module_id==this->moduleID)){
        //actions for outputs:
        if(m.feature_type == 1){
            //action for own outputs
            reset_effects(m.index);
            switch(m.function_address) {
            //switch off
            case 0x00:
                this->set_output(m.index,0,false);
                this->can_controller.ack_msg(&m,data_buffer,1);
                break;
            //switch on with value
            case 0x01:
                if(m.received_long!=0){
                    int result = (m.received_long > 256) ? 256 : m.received_long;
                    this->set_output(m.index,result,true);
                }
                else{
                    this->set_output(m.index,256,true);
                }
                this->can_controller.ack_msg(&m,data_buffer,1);
                break;
            //toggle
            case 0x02:
                blink_effect[m.index]=false;
                pulse_effect[m.index]=false;
                if(this->output_state[m.index] > 0){
                    this->set_output(m.index,0,false);
                }
                else{
                this->set_output(m.index,256,true); 
                }
                this->can_controller.ack_msg(&m,data_buffer,1);
                break;
            case 0x03:
                switch (m.received_long)
                {
                case 1:
                    if(!this->digitalIO[m.index]){
                        pulse_effect[m.index]=true;
                    }
                    break;
                case 2:
                    blink_effect[m.index]=true;
                    break;
                default:
                    this->set_output(m.index,0,false);
                    blink_effect[m.index]=false;
                    pulse_effect[m.index]=false;
                    break;
                }
                this->can_controller.ack_msg(&m,data_buffer,1);
                break;
            case 0xFF:
                //request state
                long_to_data_buffer(this->output_state[m.index]);
                this->can_controller.ack_msg(&m,data_buffer,1);
                break;
            default:
                //nothing 
                break;
            }            
        }
    }   
}


void SwitchCo::loop(){
    //read canbus
    this->can_controller.check_can_bus();
    //check if there are any can msg's ready
    if (this->can_controller.gcan_received()){
        this->on_can_msg(this->can_controller.give_last_msg());
    }
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
            //going from 0 --> 1
            press_react(i);
            }
            if(last_input_state[i]){
            //going from 1 --> 0
            release_react(i); 
            }
            last_input_state[i]=input_state[i];
        }
        else if(input_state[i]&!hold_sent[i]){
        //button is still pressed
        if(millis()-last_press[i]>long_press_val){
        //button pressed longer than 500ms
        hold_react(i);
        hold_sent[i]=1;
        }
    }
    if(millis()-last_release[i]>double_press_val && multiple_press[i]){
            if(hold_time[i]<500){
                //just a single click happend send 
                //Serial.print("single click detected hold time: ");Serial.println(hold_time[i]);
                this->can_controller.send_can_msg(this->can_controller.give_can_id(true,this->moduleID,0x00,i,0x00,false),data_buffer,1);
            }
            else{
                //Long release detected 
                //Serial.print("Long release  detected hold time: ");Serial.println(hold_time[i]);
                long_to_data_buffer(hold_time[i]);
                this->can_controller.send_can_msg(this->can_controller.give_can_id(true,this->moduleID,0x00,i,0x03,false),data_buffer,4);
            }
            multiple_press[i]=0;
        }
    }

}





