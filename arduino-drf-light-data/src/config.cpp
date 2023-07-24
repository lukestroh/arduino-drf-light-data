#include "config.h"
#include <ArduinoJson.h>
#include <RTClib.h>

#ifndef SERIAL_DEBUG
#define SERIAL_DEBUG 1
#endif // SERIAL_DEBUG

ArduinoConfig::ArduinoConfig(Eth& eth0, RTC_DS3231& ds3231):
    client(eth0),
    rtc(ds3231)
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

uint8_t* ArduinoConfig::get_octect_int(const char* address) {
    /* Make an array of octets from a string */
    char address_cpy[strlen(address)];
    strcpy(address_cpy, address); // make a char* version of the ip address for 'strtok()' function
    const char delim[2] = ".";
    char* token;
    token = strtok(address_cpy, delim);
    static uint8_t octs[4];
    uint8_t i = 0;
    while (token != nullptr) {
        octs[i] = atoi(token);
        // check to see if octet is a valid number
        if (octs[i] > 255 || octs[i] < 0) {
#if SERIAL_DEBUG
            Serial.print(F("Invalid octet value for IP address: "));
            Serial.println(octs[i]);
#endif // SERIAL_DEBUG
            break;
        }
        token = strtok(nullptr, delim);
        ++i;
    }

    // Do additional error checking?
    
    return octs;
}

void ArduinoConfig::update_datetime(const char* datetime) {
    /* Update the RTC current time */
    
    DateTime dt = DateTime(datetime);
    rtc.adjust(dt);
    
#if SERIAL_DEBUG
    char buffer[32] = "YYYY-MM-DDThh:mm:ss";
    Serial.print(F("Datetime updated: "));
    DateTime now_dt = rtc.now();
    now_dt.toString(buffer);    
    Serial.println(buffer);
#endif // SERIAL_DEBUG
}

void ArduinoConfig::update_server_ip(const char* server_ip) {
    /* Update the server IP address */
    uint8_t* octs = get_octect_int(server_ip);

    client.lan_server_ip = IPAddress(octs[0], octs[1], octs[2], octs[3]);

#if SERIAL_DEBUG
    Serial.print(F("Server IP Address updated: "));
    Serial.print(client.lan_server_ip);
    Serial.print(F(":"));
    Serial.println(client.lan_server_port);
#endif // SERIAL_DEBUG
}

void ArduinoConfig::update_server_port(const char* port_cstr) {
    /* Update the server port number */
    int port = atoi(port_cstr);
    client.lan_server_port = port;
#if SERIAL_DEBUG
    Serial.print(F("LAN server port updated: "));
    Serial.print(client.lan_server_ip);
    Serial.print(F(":"));
    Serial.println(client.lan_server_port);
#endif // SERIAL_DEBUG
}

void ArduinoConfig::update_client_ip(const char* client_ip) {
    /* Update the Arduino's IP address */
    uint8_t* octs = get_octect_int(client_ip);
    client.IP[0] = octs[0];
    client.IP[1] = octs[1];
    client.IP[2] = octs[2];
    client.IP[3] = octs[3];

    // Reconnect eth0
    client.begin_ethernet();

    // POST update to 'mcus'
}

void ArduinoConfig::update_gateway_ip(const char* gateway_ip) {
    /* Update the Eth gateway and DNS IP addresses */
    uint8_t* octs = get_octect_int(gateway_ip);
    client.GATEWAY[0] = client.DNS[0] = octs[0];
    client.GATEWAY[1] = client.DNS[1] = octs[1];
    client.GATEWAY[2] = client.DNS[2] = octs[2];
    client.GATEWAY[3] = client.DNS[3] = octs[3];

    // Reconnect eth0
    client.begin_ethernet();
}

void ArduinoConfig::update_param(const char* key, const char* value) {
    /* Specify which parameter to update */
    if (strcmp(key, "datetime") == 0) {
        update_datetime(value);
    } else if (strcmp(key, "server_ip") == 0) {
        update_server_ip(value);
    } else if (strcmp(key, "port") == 0) {
        update_server_port(value);
    } else if (strcmp(key, "client_ip") == 0) {
        update_client_ip(value);
    } else if (strcmp(key, "gateway_ip") == 0) {
        update_gateway_ip(value);
    } else {
#if SERIAL_DEBUG
        Serial.println(F("JSON key did not match any configuration parameters"));
#endif // SERIAL_DEBUG
    }
}

void ArduinoConfig::edit_params(const char* serial_data) {
    /* Determine what parameter to set up for editing the Arduino parameters */
    const int json_post_capacity { JSON_OBJECT_SIZE(16) };
    StaticJsonDocument<json_post_capacity> doc_from_serial;
    DeserializationError err = deserializeJson(doc_from_serial, serial_data);

    // If there is an error, quit the function
    if (err) {
#if SERIAL_DEBUG
        Serial.print(F("deserializeJson failed with code: "));
        Serial.println(err.f_str());
#endif // SERIAL_DEBUG
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