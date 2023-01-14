#include <Arduino.h>
#include <switchCo.h>

boolean digio[7] ={true,false,true,true,true,true,true};
SwitchCo s = SwitchCo(0x01,"test01",digio);
void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println("I'm very interested");


}

void loop() {
  s.loop();
  // s.set_output(1,0,true);
  // delay(200);                     
  // s.set_output(2,0,true); 
  // delay(200);  
  // s.set_output(1,0,false);
  // delay(200);                     
  // s.set_output(2,0,false);   
  // delay(200);  
}