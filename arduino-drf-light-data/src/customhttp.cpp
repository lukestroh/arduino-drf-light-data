#include "customhttp.h"
#include "auth.h"
#include <ArduinoJson.h>

const int json_post_capacity { JSON_OBJECT_SIZE(4) };
StaticJsonDocument<json_post_capacity> doc_POST;
char json_output_buf[128];

CustomHttp::CustomHttp(Eth& eth0): 
    /* Initialize class variables*/
    http(eth0.ard_client, eth0.lan_server_ip, eth0.lan_server_port)
{ 

}

void CustomHttp::construct_json_doc(bool pin_status, uint8_t light_pwm) {
    doc_POST["light_name"] = DEVICE_ID;
    doc_POST["datetime"] = "2023-07-14:19:33";
    doc_POST["light_status"] = pin_status;
    doc_POST["light_pwm"] = light_pwm;
    serializeJson(doc_POST, json_output_buf);
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