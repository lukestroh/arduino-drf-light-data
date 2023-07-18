#include "config.h"
#include <ArduinoJson.h>

ArduinoConfig::ArduinoConfig(Eth& eth0):
    client(eth0)
{

}

void ArduinoConfig::read_data_with_markers() {
    /* Read data from the serial port using markers */
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

void ArduinoConfig::update_datetime(const char* datetime) { /* Update the RTC current time */
}

void ArduinoConfig::update_server_ip(const char* server_ip) {
    /* Update the server IP address */
    // Parse cstr for octets
    // http://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm

    // Reconnect eth0
    client.begin_ethernet();
}

void ArduinoConfig::update_server_port(const char* port_cstr) {
    /* Update the server port number */
    int port = atoi(port_cstr);
    client.lan_server_port = port;
}

void ArduinoConfig::update_client_ip(const char* client_ip) { /* Update the Arduino's IP address */
}

void ArduinoConfig::update_gateway_ip(const char* gateway_ip) { /* Update the Eth gateway and DNS IP addresses */
}

void ArduinoConfig::update_param(const char* key, const char* value) {
    if (strmcp(key, "datetime") == 0) {
        update_datetime(value);
    } else if (strmcp(key, "server_ip") == 0) {
        update_server_ip(value);
    } else if (key == "port") {
        update_server_port(value);
    } else if (key == "client_ip") {
        update_client_ip(value);
    } else if (key == "gateway_ip") {
        update_gateway_ip(value);
    }
}

void ArduinoConfig::edit_params(const char* serial_data) {
    /* Determine what parameter to set up for editing the Arduino parameters */
    const int json_post_capacity { JSON_OBJECT_SIZE(1) };
    StaticJsonDocument<json_post_capacity> doc_from_serial;
    DeserializationError err = deserializeJson(doc_from_serial, serial_data);

    if (err) {
#if DEBUG
        Serial.print(F("deserializeJson failed with code: "));
        Serial.println(err.f_str());
#endif // DEBUG
        return;
    }

    // Get JsonObject to determine key and value pair
    JsonObject jobj = doc_from_serial.as<JsonObject>();
    for (auto key : update_keys) {
        const char* error = jobj[key];
        if (error != nullptr) {
            update_param(key, jobj[key].as<const char*>());
        }
    }
}