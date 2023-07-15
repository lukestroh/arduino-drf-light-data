/*
 * Light Controller DRF Connector
 * 
 * 
 */
#define DEBUG 1
#define LED 0

#include "EthTCP.h"
#include "auth.h"
#include "customswitch.h"
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>

// Switches
uint8_t switch_pin0 {2};
CustomSwitch switch0(switch_pin0);

// Lights
uint8_t pin_light0 {8};
uint8_t pwm_light0 {255};

// Ethernet
Eth eth0;

// HTTP client
char user_agent[] = "Arduino/Mega";
HttpClient http(eth0.local_client, eth0.lan_server_ip, eth0.lan_server_port);


// JSON Doc for POST
const int json_post_capacity = JSON_OBJECT_SIZE(4);
StaticJsonDocument<json_post_capacity> doc_POST;
char json_output_buf[128];



void construct_json_doc(bool pin_status, uint8_t light_pwm) {
    doc_POST["light_name"] = DEVICE_ID;
    doc_POST["datetime"] = "2023-07-14:19:33";
    doc_POST["light_status"] = pin_status;
    doc_POST["light_pwm"] = light_pwm;
    serializeJson(doc_POST, json_output_buf);
}

void post_http_msg() {
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

#if DEBUG
    uint16_t http_status_code = http.responseStatusCode();
    String resp = http.responseBody();
    Serial.print(F("Status code: "));
    Serial.println(http_status_code);
    Serial.print(F("Response: "));
    Serial.println(resp);
#endif // DEBUG

    http.stop();
}

void setup() {
    Serial.begin(115200);
    
    // Ethernet connections
    eth0.begin_ethernet();

}

void loop() {

    /* If button pressed, turn the lights on and send an HTTP Post */
    if (switch0.read_switch()) {
        if (LED) {
            if (!pwm_light0) {
                pwm_light0 = 255; // change to analogRead for LEDs
            } else {
                pwm_light0 = 0;
            }
            analogWrite(pin_light0, pwm_light0);
        }
        else {
            pwm_light0 = NULL; // This gets converted to 0   :(
            digitalWrite(pin_light0, !digitalRead(pin_light0));
        }

        // Send data to DB
        construct_json_doc(digitalRead(pin_light0), pwm_light0);
        post_http_msg();
    }
}
