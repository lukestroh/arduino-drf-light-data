#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <ArduinoJson.h>

const char* update_keys[] = {
    "datetime",
    "server_ip",
    "port",
    "client_ip",
    "gateway_ip"
};

int numChars = { 32 };
char receivedChars[32];
bool newData { false };

void read_data_with_markers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();
        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1; // don't pass max index value
                }
            } else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        } else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void update_datetime(const char* datetime) {
    /* Update the RTC current time */
    
}

void update_server_ip(const char* server_ip) {
    /* Update the server IP address */
    // Update eth0 value

    // Reconnect eth0
}

void update_server_port(const char* port) { /* Update the server port number */ }

void update_client_ip(const char* client_ip) { /* Update the Arduino's IP address */ }

void update_gateway_ip(const char* gateway_ip) { /* Update the Eth gateway and DNS IP addresses */ }

void update_param(const char* key, const char* value) {
    if (key == "datetime") {update_datetime(value);}
    else if (key == "server_ip") {update_server_ip(value);}
    else if (key == "port") {update_server_port(value);}
    else if (key == "client_ip") {update_client_ip(value);}
    else if (key == "gateway_ip") {update_gateway_ip(value);}
}

void edit_params(char* serial_data) {
    /* Determine what parameter to set up for editing the Arduino parameters */
    const int json_post_capacity { JSON_OBJECT_SIZE(5) };
    StaticJsonDocument<json_post_capacity> doc_from_serial;
    DeserializationError err = deserializeJson(doc_from_serial, serial_data);
#if DEBUG
    if (err) {
        Serial.print(F("deserializeJson failed with code: "));
        Serial.println(err.f_str());
        return;
    }
#endif // DEBUG
    // Get JsonObject to determine key and value pair
    JsonObject jobj = doc_from_serial.as<JsonObject>();
    for (auto key : update_keys) {
        const char* error_datetime = jobj[key];
        if (error_datetime != nullptr) {
            update_param(key, jobj[key].as<const char*>());
        }
    }    
}



#endif // CONFIG_H

