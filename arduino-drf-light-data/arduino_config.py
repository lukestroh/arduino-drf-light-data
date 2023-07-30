"""
arduino_config.py
Author: Luke Strohbehn
"""

import serial
import json
import ipaddress
import datetime as dt
import sys

class ArduinoConfigUpdater():
    def __init__(self, port: str, baudrate: int) -> None:
        self.port = port
        self.BAUDRATE = baudrate
        self.command_dict = {
            "datetime": self.update_datetime()
        }
        return
    
    def connect_to_arduino(self) -> None:
        """ Create the connection to Arduino """
        try:
            self.ser = serial.Serial(self.port, baudrate=self.BAUDRATE)
            print(self.ser)
        except (serial.serialutil.SerialException) as e:
            sys.exit(f"\nCould not connect to the device. Error: {e}\n")
        return

    
    def update_datetime(self) -> None:
        """ Update the datetime on the Arduino using the current time """
        datetime_dict = {"datetime": dt.datetime.now().strftime(r"%Y-%m-%dT%H:%M:%S.%f")} # ISO 8601 datetime format for RTClib
        datetime_json = json.dumps(datetime_dict)
        print(datetime_json)
        self.send_data(datetime_json)
        return

    def update_server_ip(self, server_ip: str) -> None:
        """ Update the IP address of the server (Django)"""
        server_ip = server_ip.strip()
        server_ip_dict = {"server_ip": server_ip}
        try:
            ipaddress.ip_address(server_ip)
        except ValueError as e:
            print(f"{e}. Please try again.")
            return
        server_ip_json = json.dumps(server_ip_dict)
        self.send_data(server_ip_json)
        return

    def update_server_port(self, port: str) -> None:
        """ Update the por of the server """
        port_dict = {"port": port.strip()}
        port_json = json.dumps(port_dict)
        self.send_data(port_json)
        return

    def update_client_ip(self, client_ip: str) -> None:
        """ Update the IP address of the Arduino """
        client_ip = client_ip.strip()
        client_ip_dict = {"client_ip": client_ip}
        try:
            ipaddress.ip_address(client_ip)
        except ValueError as e:
            print(f"{e}. Please try again.")
            return
        client_ip_json = json.dumps(client_ip_dict)
        self.send_data(client_ip_json)
        return

    def update_gateway_ip(self, gateway_ip: str) -> None:
        """ Update the IP address of the gateway and DNS """
        gateway_ip = gateway_ip.strip()
        try:
            ipaddress.ip_address(gateway_ip)
        except ValueError as e:
            print(f"{e}. Please try again.")
            return
        gateway_ip_dict = {"gateway_ip": gateway_ip}
        gateway_ip_json = json.dumps(gateway_ip_dict)
        self.send_data(gateway_ip_json)
        return
    
    def update_device_location(self, location: str) -> None:
        return
    
    def update_device_name(self, name: str, _id: str = None) -> None:
        name = name.strip()
        _id = _id.strip()
        name_id_dict = {"device_name": name, "device_id": _id}
        name_id_json = json.dumps(name_id_dict)
        self.send_data(name_id_json)
        return

    def send_data(self, data: str) -> None:
        """ Send the data via Serial to the Arduino in JSON format with markers """
        start_marker = '<'
        end_marker = '>'

        b_data = (start_marker + data + end_marker).encode()
        self.ser.write(b_data)
        return
    
    def get_device_resp():
        """ Get the Serial response from the Arduino """
        return

    def get_user_input(self) -> None:
        """ Get the command line arguments for changing Arduino parameters """
        param = sys.argv[1].strip()
        value = sys.argv[2].strip()
        # look up proper way to get optional commandline args?
        

        return


def main():
    acu = ArduinoConfigUpdater(port='com19', baudrate=115200)
    acu.connect_to_arduino()

    # Perhaps send all the data? Make a JSON with all of the arguments?
    acu.update_datetime()

    return

if __name__ == "__main__":
    main()