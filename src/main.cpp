#include <Arduino.h>
#include <switchCo.h>
#include <controllers/sk.h>
SwitchCo* s;
void setup() {
    Serial.begin(115200);
    s = new SwitchCo(0x03,"test03");
}
void loop() {
  s->loop();
}