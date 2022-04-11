#include "switchCo.h"


SwitchCo::SwitchCo(byte canID, String friendly_name):
    canID(canID),
    friendly_name(friendly_name)
{
    setup_inputs();

}
//Define input pins
void setup_inputs(){
    //Built in switch
}