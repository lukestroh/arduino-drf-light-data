#include "customhttp.h"
#include "auth.h"
#include <ArduinoJson.h>


const int json_post_capacity { JSON_OBJECT_SIZE(4) };
StaticJsonDocument<json_post_capacity> doc_lightdata;
char json_lightdata_buf[128];

const int json_put_mcu_capacity { JSON_OBJECT_SIZE(4) };
StaticJsonDocument<json_put_mcu_capacity> doc_mcu_update;
char json_mcu_update_buf[128];

CustomHttp::CustomHttp(Eth& _eth0, RTC_DS3231& _rtc): 
    /* Initialize class variables*/
    eth0(_eth0),
    http(_eth0.ard_client, _eth0.lan_server_ip, _eth0.lan_server_port),
    rtc(_rtc)
{ 

}

void CustomHttp::construct_lightdata_json(bool pin_status, uint8_t light_pwm) {
    // Create datetime string
    char dt_buffer[32] = "YYYY-MM-DDThh:mm:ss";
    Serial.print(F("Datetime updated: "));
    DateTime now_dt = rtc.now();
    now_dt.toString(dt_buffer);

    doc_lightdata["light_name"] = DEVICE_ID; // fix this, it isn't light, but MCU name here..
    doc_lightdata["datetime"] = dt_buffer;
    doc_lightdata["light_status"] = pin_status;
    doc_lightdata["light_pwm"] = light_pwm;
    serializeJson(doc_lightdata, json_lightdata_buf);
}

void CustomHttp::construct_mcu_json(const char* device_id, const char* ip_address) {
    doc_mcu_update["mcu_name"] = DEVICE_NAME;
    doc_mcu_update["ip_address"] = eth0.IP;
    doc_mcu_update["room_name"] = 1; // bedroom. Update function for this!
    doc_mcu_update["lights"] = 1; // This needs to be a json object within object for multiple lights. This also needs an update function.
    serializeJson(doc_mcu_update, json_mcu_update_buf);
}


void CustomHttp::send_http_msg(
    const char* method,
    bool lightdata,
    bool mcu_update
) {
    /* Submit an http request with some specific headers */
    char* data_buf;
    char* url;
    if (lightdata) {
        data_buf = json_lightdata_buf;
        url = lightdata_url;
    }
    else if (mcu_update) {
        data_buf = json_mcu_update_buf;
        url = mcu_update_url;
    }

    // Token string
    char token_arr[48];
    strcpy(token_arr, "Token ");
    strcat(token_arr, TOKEN);

    // HTTP request
    http.beginRequest();

    if (strcmp(method, "POST") == 0 || strcmp(method, "post") == 0) {
        http.post(url);
    }
    else if (strcmp(method, "PUT") == 0 || strcmp(method, "put") == 0) {
        http.put(url);
    }
    else if (strcmp(method, "GET") == 0 || strcmp(method, "get") == 0) {
        http.get(url);
    }

    http.sendHeader("Content-Type", "application/json");
    http.sendHeader("Authorization", token_arr);
    http.sendHeader("User-Agent", user_agent);
    http.sendHeader("Connection", "close");

    // Send specific data
    if (lightdata) {
        http.sendHeader("Content-Length", measureJson(doc_lightdata)); // maybe need to make these global / static like up above?
        http.beginBody();
        http.print(json_lightdata_buf);
    }
    else if (mcu_update) {
        http.sendHeader("Content-Length", measureJson(doc_mcu_update));
        http.beginBody();
        http.print(json_mcu_update_buf);
    }
    http.endRequest();

    // Get HTTP Response if connected via Serial.
#if SERIAL_DEBUG
    uint16_t http_status_code = http.responseStatusCode();
    String resp = http.responseBody();
    Serial.print(F("Status code: "));
    Serial.println(http_status_code);
    Serial.print(F("Response: "));
    Serial.println(resp);
#endif // SERIAL_DEBUG

    http.stop();
}