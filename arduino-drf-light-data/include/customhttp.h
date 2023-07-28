#ifndef LOCALHTTP_H
#define LOCALHTTP_H

#include "EthTCP.h"
#include <ArduinoHttpClient.h>
#include <RTClib.h>

class CustomHttp {
    private:
        // URLS
        char* lightdata_url = "/api/lightdata";
        char* mcu_update_url = "/api/mcus"

        // Eth
        Eth& eth0;

        // HTTP client
        char user_agent[16] = "Arduino/Mega";
        HttpClient http;

        // RTC
        RTC_DS3231& rtc;



    public:
        CustomHttp();
        CustomHttp(Eth& _eth0, RTC_DS3231& _rtc);
        void construct_lightdata_json(bool pin_status, uint8_t light_pwm);
        void construct_mcu_json(const char* device_id, const char* ip_address);
        void send_http_msg(const char* method, const char* url, bool lightdata=false, bool mcu_update=false);
};



#endif // LOCALHTTP_H