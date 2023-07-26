#ifndef LOCALHTTP_H
#define LOCALHTTP_H

#include "EthTCP.h"
#include <ArduinoHttpClient.h>
#include <RTClib.h>

class CustomHttp {
    private:
        // HTTP client
        char user_agent[16] = "Arduino/Mega";
        HttpClient http;

        // RTC
        RTC_DS3231& rtc;


    public:
        CustomHttp();
        CustomHttp(Eth&);
        void construct_json_data_doc(bool pin_status, uint8_t light_pwm);
        void construct_json_mcu_doc(const char* device_id, const char* ip_address);
        void post_http_msg();
        void put_http_msg();


};



#endif // LOCALHTTP_H