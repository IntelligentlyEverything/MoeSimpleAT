- English
- [简体中文](README_zhcn.md)
- [繁體中文](README_zhtw.md)
- [日本語](README_jp.md)

# MoeSimpleAT
[![License](https://img.shields.io/badge/license-LGPL-blue.svg)](LICENSE)

MoeSimpleAT is a simple instruction set based on AT instructions, which is designed to be customizable, flexible, expandable, and easy to use for everyone.

MoeSimpleAT is different from other AT instruction sets in that it is not specific to a single device. Through simple and customizable AT instructions and SHELL(msh) instructions, users can easily implement various functions.

## Features

- Easy to use: The instruction set design of MoeSimpleAT is simple, the number of instructions in the instruction set is small, and the instruction syntax of the instruction set is simple.
- Flexible Expansion: The instruction set of MoeSimpleAT can be easily expanded, and users can add their own instructions according to their needs.

## Usage
1. Download and install Arduino IDE

Go to the [Arduino website](https://www.arduino.cc/en/software), download and install Arduino IDE.
2. Install MoeSimpleAT library

Go to the Arduino library directory, usually "Documents/Arduino/libraries"

Download MoeSimpleAT library using the `git clone` command:
``` shell
git clone https://github.com/Moe-Simple/MoeSimpleAT.git
```
3. Open Arduino IDE, select the development board and port

Optional boards:
  - ESP8266
  - ESP32
  - Air001(AirMCU)
4. Import and store files:
  ``` Arduino
  #include <MoeSimpleAT.h>
  ```
5. Start your development journey!

## Basic Usage
Basic Example Code
``` Arduino
#include <MoeSimpleAT.h>

#define SERIAL_BAUD_RATE 115200 // Set serial port baud rate

void setup() {
    atSerial->begin(SERIAL_BAUD_RATE); // Open serial port
    initATCommands(); // Initialize AT instruction set
}

void loop() {
    handleATCommands(); // Handle AT commands
}
```

With just a few lines of code, you can easily develop AT applications for your device. When you compile the code, you will see the following message in the serial port:
``` shell
MoeSimple AT System by YooTrans.
ready
```
This means that the AT instruction set has been initialized successfully.

### Built-in AT Commands
The library contains some built-in commands, you can enter `AT+HELP` or `AT+?` to get help, and the following is a list of built-in commands:

- AT: Test AT startup
- AT+RST: Restart device
- AT+GMR: Get version information
- AT+RESTORE: Restore device (defined by onRestore(\<restore function\>); if not defined, only returns OK)
- AT+UART?: Get current serial port baud rate
- AT+UART=xxx: Set serial port baud rate
- AT+SYSRAM?: Get system memory usage (not supported for external PSRAM)
- AT+SHELL: Enter SHELL mode as an interactive terminal, input exit to exit
- AT+LOG: Enter log output mode, only output logs, do not process AT commands, input EXIT to exit

### Built-in SHELL Commands
- echo \<string\>: Output string to serial port
- free [-b|-k|-m] [-t] [-s delay]: Display memory usage, same as Linux free command, supports internal RAM and external PSRAM, -b: bytes, -k: kilobytes, -m: megabytes, -t: display total, -s: refresh interval (seconds)
- reboot: Restart the device with the same function as the `AT+RST` command in AT mode
- shutdown: Turn off the device and set `wakeupConfigured = true;`Set up wake-up related logic.
- exit: Exit SHELL mode
- help: display help information

## Customize AT commands
You can register your own AT commands by calling the `registerATCommand(<instructions>, <callback>, <help message>)` function in your program.

The callback function should have the following signature:
``` Arduino
void myCallback(String args) {
    // Processing instruction parameters
    // output result
    atSerial->println("OK"); // It is necessary to output OK to the serial port as a termination and compatibility with AT devices.
}
```

## Customize SHELL command
You can register your own SHELL commands by calling the `registerShellCommand(<instructions>, <callback>, <help message>)` function in your program.
The callback function should have the following signature:
``` Arduino
void myShellCallback(String args) {
    // Processing instruction parameters
    // output result
}
```

## Contribution
Welcome to contribute! Please read [CONTRIBUTING.md](CONTRIBUTING.md) to learn how to participate in project development.

- Fork the repository
- Create your feature branch (git checkout -b feature/AmazingFeature)
- Submit your changes (git commit -m 'Add some AmazingFeature')
- Push to branch (git push origin feature/AmazingFeature)
- Open a Pull Request

## Version Updates
View the [CHANGELOG.md](CHANGELOG.md) file to learn about the version update history.

## Author
- [MoeCinnamo](https://github.com/MoeCinnamo)

## Acknowledgments
- Thank you to the contributors for participating in the development of this project.

## LICENSE
This project is open sourced under the [LGPLv2.1](LICENSE) license.