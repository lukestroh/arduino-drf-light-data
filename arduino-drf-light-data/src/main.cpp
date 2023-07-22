#ifndef SERIAL_DEBUG
#define SERIAL_DEBUG 1
#endif //SERIAL_DEBUG

#ifndef LED
#define LED 0
#endif // LED

#include "EthTCP.h"
#include "config.h"
#include "customhttp.h"
#include "customswitch.h"
#include <Arduino.h>
#include <RTClib.h>
#include <Wire.h>

// Switches
uint8_t switch_pin0 { 2 };
CustomSwitch switch0(switch_pin0);

// Lights
uint8_t pin_light0 { 8 };
uint8_t pwm_light0 { 255 };
bool pin_status { false };

// RTC
RTC_DS3231 rtc;

// Ethernet
Eth eth0;

// HTTP
CustomHttp custom_http(eth0);

// Config
ArduinoConfig conf(eth0, rtc);

/****************************************************************
 * Main
 **************************************************************/

void setup() {
#if SERIAL_DEBUG
    Serial.begin(115200);
#endif // SERIAL_DEBUG

    // Ethernet connections
    eth0.begin_ethernet();

    // RTC
    Wire.begin();
    rtc.begin();
    rtc.now();
}

void loop() {
    // If button pressed, turn the lights on and send an HTTP Post
    if (switch0.read_switch()) {
        if (LED) {
            if (!pwm_light0) {
                pwm_light0 = 255; // change to analogRead for LEDs
            } else {
                pwm_light0 = 0;
            }
            analogWrite(pin_light0, pwm_light0);
        } else {
            pwm_light0 = 0;
            pin_status = true - pin_status;
            digitalWrite(pin_light0, !pin_status);
        }

        // Send data to DB
        custom_http.construct_json_doc(pin_status, pwm_light0);
        custom_http.post_http_msg();
    }

    // Read any new data
    conf.read_data_with_markers();
        
    // Determine any configuration updates
    if (conf.newData) {
#if SERIAL_DEBUG
        Serial.println(conf.receivedChars);
#endif // SERIAL_DEBUG
        conf.edit_params(conf.receivedChars);
        conf.newData = false;
    }

}