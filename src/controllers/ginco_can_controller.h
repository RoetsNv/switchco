#include <stdlib.h>
#include <Arduino.h>
#include <CAN.h>
struct GCanMessage {
  long extended_id;
  boolean event;
  byte source_module_id;
  boolean linked; 
  boolean ack;
  byte feature_type;
  byte index;
  byte function_address;
  size_t buffer_size;
};
#include <functional>
#define GCAN_CALLBACK_SIGNATURE std::function<void(GCanMessage)> callback // callback(is_event,moduleID,is_linked,is_ack,feature,index,function_adr)


class GCANController {
    private:
    byte canID;
    byte output_buffer[8]; //buffer to write data to when sending msg
    byte receive_buffer[8];
    GCanMessage* last_msg;
    byte interested_in[10]; //list of module ID's the module should listen to
    GCAN_CALLBACK_SIGNATURE;
    public:
        GCANController(byte canID);
        GCANController();
        void setup();
        void send_can_msg(long can_id,const byte *data,size_t buffer_size);
        long give_can_id(byte feature_type,byte index_number,byte func_id);
        void add_moduleID(byte moduleID); //add module ID to interested list
        void check_if_msg();
        void set_callback(GCAN_CALLBACK_SIGNATURE);
        void handle_can_msg(int packet_size);
}; 