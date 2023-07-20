#ifndef LOCALHTTP_H
#define LOCALHTTP_H

#include "EthTCP.h"
#include <ArduinoHttpClient.h>
#include <DS3231.h>

class CustomHttp {
    private:
        // HTTP client
        char user_agent[16] = "Arduino/Mega";
        HttpClient http;     

    public:
        // CustomHttp();
        CustomHttp(Eth&);
        void construct_json_doc(bool pin_status, uint8_t light_pwm);
        void post_http_msg();


};



#endif // LOCALHTTP_H