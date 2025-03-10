# A GUI and IoT Device Framework

## Introduction

This project allows a user to control “internet of things” (IoT) devices via a desktop computer graphical user interface (GUI). The GUI was created using the GUI framework _PyQt6_ and features serial port configuration, sending of commands, and a terminal. The IoT devices are two _ESP32-C6_ microcontrollers; one is considered a “leader” and the other is considered a “follower”. The leader is connected to the GUI via a serial port and USB cable, and the follower is connected to the leader via the network protocol _Zigbee_. The IoT devices use the real-time operating system (RTOS) _FreeRTOS_.

This project was created for the "ECE 406: Projects" class at Oregon State University with the purpose of learning new skills.

## Design Choice Rational

**GUI:** GUIs are important because they are used everywhere and are accessible to non-tech-savvy users. PyQt6 was chosen because it is light-weight, free, and the engineering team (just me, [@TravisFredrickson](https://www.github.com/TravisFredrickson)) had previous experience using it.

**RTOS:** RTOSs are important for safety critical applications, for organizing code into separate tasks, and because they are common in industry. FreeRTOS was chosen because it is popular and (you’ll never guess this) free.

**ESP32-C6:** The ESP32-C6 was chosen as the microcontroller because ESPs are popular, it has many network peripherals (2.4 GHz Wi-Fi 6, Bluetooth 5 (LE), Zigbee, and Thread) and a cheap (~$10) development board that fits well on the common BB830 breadboard.

**Network:** Zigbee was chosen as the communication method and network protocol between microcontroller devices because it seemed like the simplest method available that easily allowed many devices on the network.

## Challenges, Lessons Learned, and Takeaways

**Build system and workspace setup:** This can get complicated, especially when mixing build systems for C and Rust.

**Messaging format between GUI and microcontrollers:** This was kept simple, but I can imagine how this can become complicated to make complete and consistent in larger, more complex systems.

**Hardware input debouncing:** Solved via software rather than via hardware with an RC circuit. This freed up breadboard space and allowed for wires to be used as “buttons”.

**Development time and task management:** It is important to stay on track, know what steps are next, and know when to stop. I have gained a greater appreciation for (good) managers.

**Ready to learn more:** I am ready to learn from more experience engineers how to do things better, such as manage RTOSs and create GUIs.

## Future Improvements

**GUI:**
- Consider using a different GUI framework; PyQt6 may not be the best option. For example, one issue with PyQt6 is that some styling is within the Python file and some is within the CSS file, which is not ideal. Alternative GUI framework options include LVGL, Qt QML, and Tauri. Alternatively, a website could be used.
- Improve the GUI by adding color theme settings, controlling many individual IoT devices, and making it cross-platform (currently only for desktop computers).
- Reconcile using either UART, ESP log, or print commands in the microcontroller code for transmitting to GUI.

**Networking:**
- Verify the network can seamlessly handle adding many new devices, dropouts, and errors.
- If continuing with existing Zigbee code, improve code readability and ease of sending different data types. I was not the most impressed with the example code from Espressif for their implementation of Zigbee.

**Purpose and use:** Make the project do something specific. For example, control a pen plotter or a smart thermostat. Currently, the project is only a “framework” of sorts.

## Pictures

1. Nice (maybe edited) showcase picture.
2. Block diagram.
3. GUI.
4. ESP32s and breadboard.
