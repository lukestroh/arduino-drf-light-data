#ifndef CONFIG_H
#define CONFIG_H

#include "device_params.h"
#include "EthTCP.h"
#include <RTClib.h>
#include <Arduino.h>

class ArduinoConfig {
    private:
        // Update char* keys
        const char* lightdata_update_keys[5] = { "datetime", "server_ip", "port", "client_ip", "gateway_ip" };

        DeviceParams& dps;

        // Eth0 client
        Eth& client;

        // RTC
        RTC_DS3231& rtc;

        // Private methods
        void update_datetime(const char* datetime);
        uint8_t* get_octect_int(const char* address);
        void update_server_ip(const char* server_ip);
        void update_server_port(const char* port_cstr);
        void update_client_ip(const char* client_ip);
        void update_gateway_ip(const char* gateway_ip);
        void update_device_location(const char* location);
        void update_device_name(const char* device_name);
        void update_device_id(const char* device_id);
        void update_param(const char* key, const char* value);

        void add_light();
        void remove_light();
        void update_light_id_list();

        void _change_mcu_url_call();

        
        

    public:
        // Receive data buffer
        int numChars = { 64 };
        char receivedChars[64];
        bool newData { false };

        // Constructor
        ArduinoConfig();
        ArduinoConfig(DeviceParams&, Eth&, RTC_DS3231&);

        // Public methods
        void read_data_with_markers();
        void edit_params(const char* serial_data);
};


#endif // CONFIG_H