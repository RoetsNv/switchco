#include <Arduino.h>
#include <switchCo.h>
#include <controllers/sk.h>
boolean digio[7] ={false,false,false,true,true,true,true};
SwitchCo s = SwitchCo(0x02,"test02",digio);
void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println("I'm very interested");
}
void loop() {
  s.loop();
}