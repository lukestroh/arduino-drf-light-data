#ifndef DEVICEPARAMS_H
#define DEVICEPARAMS_H

struct DeviceParams {
    unsigned long DEVICE_ID {1}; 
    char DEVICE_NAME[16]  {"mcu0"};
    char DEVICE_LOCATION[32] {"bedroom"};
    int DEVICE_LIGHTS_IDS[2] = {1};
};
// :)
#endif // DEVICEPARAMS_H