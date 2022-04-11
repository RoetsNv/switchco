#include <stdlib.h>
#include <Arduino.h>
#include <CAN.h>

class GCANController {
    public:
        GCANController(double baudrate);
        void setup();
        void send_can_msg(long can_id,const byte *data,size_t buffer_size);
};