#include "customhttp.h"
#include "auth.h"
#include <ArduinoJson.h>


const int json_post_capacity { JSON_OBJECT_SIZE(4) };
StaticJsonDocument<json_post_capacity> doc_POST;
char json_output_buf[128];

const int json_put_mcu_capacity { JSON_OBJECT_SIZE(4) };
StaticJsonDocument<json_put_mcu_capacity> doc_PUT_MCU;
char json_mcu_update_buf[128];

CustomHttp::CustomHttp(Eth& eth0, RTC_DS3231& _rtc): 
    /* Initialize class variables*/
    http(eth0.ard_client, eth0.lan_server_ip, eth0.lan_server_port),
    rtc(_rtc)
{ 

}

void CustomHttp::construct_json_data_doc(bool pin_status, uint8_t light_pwm) {
    // Create datetime string
    char dt_buffer[32] = "YYYY-MM-DDThh:mm:ss";
    Serial.print(F("Datetime updated: "));
    DateTime now_dt = rtc.now();
    now_dt.toString(dt_buffer);

    doc_POST["light_name"] = DEVICE_ID; // fix this, it isn't light, but MCU name here..
    doc_POST["datetime"] = dt_buffer;
    doc_POST["light_status"] = pin_status;
    doc_POST["light_pwm"] = light_pwm;
    serializeJson(doc_POST, json_output_buf);
}

void CustomHttp::construct_json_mcu_doc(const char* device_id, const char* ip_address) {
    doc_PUT_MCU["mcu_name"] = DEVICE_NAME;
    doc_PUT_MCU["ip_address"] = eth0.IP;
    doc_PUT_MCU["room_name"] = 1; // bedroom. Update function for this!
    doc_PUT_MCU["lights"] = 1; // This needs to be a json object within object for multiple lights. This also needs an update function.
}

void CustomHttp::post_http_msg() {
    // Token string
    char token_arr[48];
    strcpy(token_arr, "Token ");
    strcat(token_arr, TOKEN);

    http.beginRequest();
    http.post("/api/lightdata");
    http.sendHeader("Content-Type", "application/json");
    http.sendHeader("Authorization", token_arr);
    http.sendHeader("User-Agent", user_agent);
    http.sendHeader("Connection", "close");
    http.sendHeader("Content-Length", measureJson(doc_POST));
    http.beginBody();
    http.print(json_output_buf);
    http.endRequest();

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

CustomHttp::put_http_mcu_update() {
    // Token string
    char token_arr[48];
    strcpy(token_arr, "Token ");
    strcat(token_arr, TOKEN);

    http.beginRequest();
    http.put("/api/mcus");
    http.sendHeader("Content-Type", "application/json");
    http.sendHeader("Authorization", token_arr);
    http.sendHeader("User-Agent", user_agent);
    http.sendHeader("Connection", "close");
    http.sendHeader("Content-Length", measureJson(doc_PUT_MCU));
    http.beginBody();
    http.print(json_mcu_update_buf);
    http.endRequest();

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