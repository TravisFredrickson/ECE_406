# An ESP32-C6-DevKitC-1 Program

## Description

This program controls the LEDs on two "ESP32-C6-DevKitC-1 v1.2" development boards; one is a "leader" and the other is a "follower". It is intended to be used with the accompanying GUI.

## Hardware Required

- Two ESP32-C6-DevKitC-1 v1.2 development boards.
- Two USB cables for power and programming.

## Build Instructions

1. **Configure the Project:** Before project configuration and build, make sure to set the correct chip target using `idf.py --preview set-target TARGET` command.
2. **Erase the NVRAM:** Before flash it to the board, it is recommended to erase NVRAM if user doesn't want to keep the previous examples or other projects stored info using `idf.py -p PORT erase-flash`.
3. **Build and Flash:** Build the project, flash it to the board, and start the monitor tool to view the serial output. One way to do this is by selecting the `ESP-IDF: Build, Flash, and Monitor` command in Visual Studio Code with the ESP-IDF extension. Another way to do this is by running `idf.py -p PORT flash monitor` (to exit the serial monitor, type `Ctrl-]`).

## Example Output

```

```

