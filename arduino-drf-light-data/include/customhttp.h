#ifndef LOCALHTTP_H
#define LOCALHTTP_H

#include "device_params.h"
#include "EthTCP.h"
#include <ArduinoHttpClient.h>
#include <RTClib.h>

class CustomHttp {
    private:
        // URLS
        char* lightdata_url;
        char* mcu_update_base_url;
        char* mcu_update_url;

        // Device parameters struct
        DeviceParams& dps;

        // Eth
        Eth& eth0;

        // HTTP client
        char user_agent[16] = "Arduino/Mega";
        HttpClient http;

        // RTC
        RTC_DS3231& rtc;



    public:
        CustomHttp();
        CustomHttp(DeviceParams& _device_params, Eth& _eth0, RTC_DS3231& _rtc);
        void construct_lightdata_json(bool pin_status, uint8_t light_pwm);
        void construct_mcu_json(const char* device_id, const char* ip_address);
        void send_http_msg(const char* method, bool lightdata=false, bool mcu_update=false);
};



#endif // LOCALHTTP_H