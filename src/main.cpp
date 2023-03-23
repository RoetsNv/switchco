#include <Arduino.h>
#include <switchCo.h>
#include <controllers/sk.h>
boolean digio[7] ={false,false,false,true,true,true,true};
SwitchCo* s;
void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
    s = new SwitchCo(0x03,"test03");
}
void loop() {
  s->loop();
}