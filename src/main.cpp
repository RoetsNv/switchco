#include <Arduino.h>
#include <switchCo.h>
#include <controllers/sk.h>
boolean digio[7] ={false,false,false,true,true,true,true};
SwitchCo s = SwitchCo(0x02,"test02",digio);
sk pixel=sk();
void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
    Serial.println("I'm very interested");
    pixel.begin(32,1);
    pixel.color(0,70,200,0,0,5);
    pixel.show();

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