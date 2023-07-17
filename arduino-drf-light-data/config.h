#ifndef CONFIG_H
#define CONFIG_H

#include "EthTCP.h"
#include <Arduino.h>

class ArduinoConfig {
    private:
        // Update char* keys
        const char* update_keys[5] = { "datetime", "server_ip", "port", "client_ip", "gateway_ip" };

        // Eth0 client
        Eth client;

        // Private methods
        void update_datetime(const char* datetime);
        void update_server_ip(const char* server_ip);
        void update_server_port(const char* port_cstr);
        void update_client_ip(const char* client_ip);
        void update_gateway_ip(const char* gateway_ip);
        void update_param(const char* key, const char* value);

    public:
        // Receive data buffer
        int numChars = { 32 };
        char receivedChars[32];
        bool newData { false };

        // Constructor
        ArduinoConfig(Eth&);

        // Public methods
        void read_data_with_markers();
        void edit_params(const char* serial_data);

};


#endif // CONFIG_H