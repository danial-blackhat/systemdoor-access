# SystemDoor Access Control 🔒

[![Arduino Nano BLE33](https://img.shields.io/badge/Arduino-Nano_BLE33-blue)](https://www.arduino.cc/en/Guide/NANO33BLE)
[![License](https://img.shields.io/badge/License-MIT-green)](https://opensource.org/licenses/MIT)
![Project Status](https://img.shields.io/badge/status-active-brightgreen)

![SystemDoor Demo](https://github.com/danial-blackhat/systemdoor-access/blob/main/picture_1.jpg)

A secure access control system using RFID authentication and facial recognition, built for hardware learning and prototyping. This version uses the Arduino Nano BLE33 microcontroller and does not include any web interface or database.

## Features ✨

- **Dual Authentication** – RFID card + facial recognition
- **Hardware Integration** – Arduino Nano BLE33 controls door locking
- **Multi-user Support** – Multiple RFID cards

## Hardware Requirements 🛠️

| Component              | Specification                     |
|------------------------|-----------------------------------|
| Microcontroller        | Arduino Nano BLE33                |
| RFID Reader            | RC522 RFID Module                 |
| Lock Mechanism         | 12V Electric Strike or Solenoid   |
| Power Supply           | 12V 2A DC Power Adapter           |
| Relay Module           | 5V Single Channel Relay           |


## Notes

- No web interface or online dashboard is included.
- No database; logs are local 
- All authentication and control are handled by hardware modules.

---

> Created by Wan Muhammad Danial for hardware prototyping and learning.
