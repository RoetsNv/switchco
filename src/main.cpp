#include <Arduino.h>
#include <switchCo.h>

boolean digio[8] ={true,true,true,true,true,true,true,true};
SwitchCo s = SwitchCo(0x01,"test01",digio);
void setup() {
  // put your setup code here, to run once:
  
}

void loop() {
  s.loop();
}