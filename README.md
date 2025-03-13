# A GUI and IoT Device Framework

![Cover image.](<./Images/Cover Image.png>)

## Description

This project allows a user to control “internet of things” (IoT) devices via a desktop computer graphical user interface (GUI). The GUI was created using the GUI framework _PyQt6_ and features serial port configuration, sending of commands, and a terminal. The IoT devices are two _ESP32-C6_ microcontrollers; one is considered a “leader” and the other is considered a “follower”. The leader is connected to the GUI via a serial port and USB cable, and the follower is connected to the leader via the network protocol _Zigbee_. The IoT devices use the real-time operating system (RTOS) _FreeRTOS_.

This project was created for the "ECE 406: Projects" class at Oregon State University with the purpose of learning new skills and being a portfolio project.

## Installation

1. Get two "ESP32-C6-DevKitC-1" development boards, two USB-C cables, and a computer (preferably with Ubuntu).
2. Follow [Espressif's ESP32-C6 "Get Started" instructions](<https://docs.espressif.com/projects/esp-idf/en/v5.2.5/esp32c6/get-started/index.html>). Get Visual Studio Code with the ESP-IDF extension.
3. Set up the leader.
    1. Open the `ESP32-C6 Leader` directory in Visual Studio Code.
    2. Connect a ESP32-C6-DevKitC-1 to the computer.
    3. Configure ESP-IDF for ESP32-C6 target, UART flash method, and the correct port.
    4. Build and flash.
4. Set up the follower.
    1. Open the `ESP32-C6 Follower` directory in Visual Studio Code.
    2. Connect a ESP32-C6-DevKitC-1 to the computer.
    3. Configure ESP-IDF for ESP32-C6 target, UART flash method, and the correct port.
    4. Build and flash.
5. Set up the GUI.
    1. Open a terminal in the `GUI/sources` directory.
    2. Create a Python virtual environment using `python3 -m venv .venv`.
    3. Active the Python virtual environment using `source .venv/bin/activate`.
    4. Install Python libraries using `pip install -r python_requirements.txt`.
    5. Create an executable using `pyinstaller ./main.py --onefile --windowed --distpath ../ --name GUI_Executable`. This creates three items: `build`, `GUI_Executable`, and `GUI_Executable.spec`. `build` and `GUI_Executable.spec` can be safely deleted.
    6. The GUI executable has been created and can now be executed. Congratulations!
6. Start GUI, leader, and follower.
    1. Open GUI.
    2. Connect the leader to the computer.
    3. Refresh ports.
    4. Connect to newly available port (the leader) at 115200 baud rate.
    5. Connect the follower to the computer.
    6. You can now send commands via the buttons.
7. You did it! Congratulations!
8. ...
9. Oh, did something not work? See [this](<https://en.wikipedia.org/wiki/Rubber_duck_debugging>) for troubleshooting.

## Design Choice Rational

**GUI:** GUIs are important because they are used everywhere and are accessible to non-tech-savvy users. PyQt6 was chosen because it is light-weight, free, and the engineering team (just me, [@TravisFredrickson](https://www.github.com/TravisFredrickson)) had previous experience using it.

**RTOS:** RTOSs are important for safety critical applications, for organizing code into separate tasks, and because they are common in industry. FreeRTOS was chosen because it is popular and (you’ll never guess this) free.

**ESP32-C6:** The ESP32-C6 was chosen as the microcontroller because ESPs are popular, it has many network peripherals (2.4 GHz Wi-Fi 6, Bluetooth 5 (LE), Zigbee, and Thread) and a cheap (~$10) development board (ESP32-C6-DevKitC-1) that fits well on the common BB830 breadboard.

**Network:** Zigbee was chosen as the communication method and network protocol between microcontroller devices because it seemed like the simplest method available that easily allowed many devices on the network.

## Challenges, Lessons Learned, and Takeaways

**Mixing C and Rust:** This was attempted, but proved to be too difficult. The first step is to use the correct preprocessor derectives so that the compiled ABI for the C and Rust could are of the same format, which is easy enough. The second step of mixing C, Rust, and ESP build systems (CMake, Cargo, and ESP-IDF), linking libraries, and more, is the difficult part. There were too many erroneous and non-descriptive issues, so I was unable to get this to work. I want to attempt this again in the future.

**Messaging format between GUI and microcontrollers:** This was kept simple, but I can imagine how this can become complicated to make complete and consistent in larger, more complex systems.

**Hardware input debouncing:** Solved via software rather than via hardware with an RC circuit. This freed up breadboard space and allowed for wires to be used as “buttons”.

**Development time and task management:** It is important to stay on track, know what steps are next, and know when to stop. I have gained a greater appreciation for (good) managers. Time spent was accurately tracked to be about 140 hours, or about 3.5 weeks of full-time work if at 40 hours per week.

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

## Images

Below are the following images:
1. **Cover image:** Shows the technologies used, the GUI, and an ESP32-C6 development board.
2. **Block diagram.**
3. **GUI.**
4. **ESP32-C6s on a breadboard:** Shows the leader device's LED turning red, yellow, and green, and the follow device's LED turning white.

### 1. Cover Image

![Cover image.](<./Images/Cover Image.png>)

### 2. Block Diagram

![Block diagram.](<./Images/Block Diagram.png>)

### 3. GUI

![GUI.](<./Images/GUI.png>)

### 4. ESP32-C6s on a Breadboard

![ESP32-C6s.](<./Images/ESP32-C6s.png>)

## References and Resources

### ESP32-C6

- [ESP32-C6: Get Started](<https://docs.espressif.com/projects/esp-idf/en/v5.2.5/esp32c6/get-started/index.html>).
- [ESP32-C6-DevKitC-1 v1.2](<https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32c6/esp32-c6-devkitc-1/user_guide.html#user-guide-c6-devkitc-1-v1-2-board-front>).

### FreeRTOS

- [Espressif: FreeRTOS (IDF)](<https://docs.espressif.com/projects/esp-idf/en/latest/esp32c6/api-reference/system/freertos_idf.html>).
- [FreeRTOS](<https://www.freertos.org/>).

### UART

- [Espressif: API Reference: UART](<https://docs.espressif.com/projects/esp-idf/en/v5.2.5/esp32c6/api-reference/peripherals/uart.html>).
- [Espressif: Examples: UART](<https://github.com/espressif/esp-idf/tree/master/examples/peripherals/uart>).

### Zigbee

- [Espressif: Examples: Zigbee](<https://github.com/espressif/esp-idf/tree/master/examples/zigbee>).
- [Espressif: Zigbee SDK Documentation](<https://docs.espressif.com/projects/esp-zigbee-sdk>).
- [Espressif: Zigbee SDK Repository](<https://github.com/espressif/esp-zigbee-sdk>).

### PyQt6

- [PyQt6 Documentation](<https://doc.qt.io/qt-6/>).

### Rust

**Documentation:**
- [The Embedded Rust Book: A little Rust with your C](<https://docs.rust-embedded.org/book/interoperability/rust-with-c.html>).
- [The Rust on ESP Book](<https://docs.esp-rs.org/book/introduction.html>).
- [Mixed C and Rust ESP-IDF Example Program](<https://github.com/esp-rs/esp-idf-template/blob/master/README-cmake.md>).
- [The Smallest #![no_std] Program](<https://docs.rust-embedded.org/embedonomicon/smallest-no-std.html>).
- [Rust Support for Target Architectures](<https://rust-lang.github.io/rustup-components-history/riscv32imac-unknown-none-elf.html>).

**Build system tools:**
- [cbindgen](<https://github.com/mozilla/cbindgen>).
- [Corrosion](<https://corrosion-rs.github.io/corrosion/introduction.html>).

**Blogs:**
- [Daniel Mangum: RISC-V Bytes: Rust Cross-Compilation](<https://danielmangum.com/posts/risc-v-bytes-rust-cross-compilation/>).
- [The Embedded Rustacean: Rust FFI and cbindgen: Integrating Embedded Rust Code in C](<https://blog.theembeddedrustacean.com/rust-ffi-and-cbindgen-integrating-embedded-rust-code-in-c>).
- [Manish Goregaokar: Integrating Rust and C++ in Firefox](<https://manishearth.github.io/blog/2021/02/22/integrating-rust-and-c-plus-plus-in-firefox/>).

