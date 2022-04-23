#include <stdlib.h>
#include <Arduino.h>
#include <CAN.h>

class GCANController {
    private:
    byte canID;

    public:
        GCANController(byte canID);
        void setup();
        void send_can_msg(long can_id,const byte *data,size_t buffer_size);
        long give_can_id(byte feature_type,byte index_number,byte func_id);
}; 