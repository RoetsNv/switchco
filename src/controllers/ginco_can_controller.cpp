#include "ginco_can_controller.h"

GCANController::GCANController(byte canID):
canID(canID)
{
    CAN.setPins(35, 5);
    // start the CAN bus at 500 kbps
    if (!CAN.begin(500E3)) {
        Serial.println("Starting CAN failed!");
        while (1);
    }
}
GCANController::GCANController()
{
    CAN.setPins(35, 5);
    // start the CAN bus at 500 kbps
    if (!CAN.begin(500E3)) {
        Serial.println("Starting CAN failed!");
        while (1);
    }
}


long GCANController::give_can_id(byte feature_type,byte index_number,byte func_id){
  //Event: 0x20000000 Action: 0x00
  long can_id=0x4000000;
  //Set module id
  long mod_id=0;mod_id+=canID;
  mod_id=mod_id<<18;
  can_id=can_id+mod_id;
  //Set 8 bit feature address
  feature_type=feature_type<<5;
  feature_type+=index_number;
  long feat_interf=0;feat_interf+=feature_type;
  feat_interf=feat_interf<<8;
  feat_interf+=func_id;
  can_id+=feat_interf;
  char buffer[15];
  ltoa(can_id, buffer, 16);
  Serial.println(buffer);
  return can_id;
}

void GCANController::send_can_msg(long can_id,const byte *data,size_t buffer_size){
   CAN.beginExtendedPacket(can_id);
   CAN.write(data,buffer_size);
   CAN.endPacket();
}
