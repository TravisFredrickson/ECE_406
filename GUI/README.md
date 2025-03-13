# A GUI

## Description

This is a graphical user interface (GUI) application made for communicating via a serial port with an ESP32-C6 microcontroller. It is made using Python and primarily the `PyQt6` library.

The following video is helpful for `pyinstaller`: https://youtu.be/2X9rxzZbYqg?si=j0R_ymKh-65VNxRO

## Build Instructions

It is assumed that the reader is familiar with programming, that Ubuntu is the operating system (OS), and that `python3` is installed.

1. Open a terminal in the `sources` directory.
2. Create a Python virtual environment using `python3 -m venv .venv`.
3. Active the Python virtual environment using `source .venv/bin/activate`.
4. Install Python libraries using `pip install -r python_requirements.txt`.
5. Create an executable using `pyinstaller ./main.py --onefile --windowed --distpath ../ --name GUI_Executable`. This creates three items: `build`, `GUI_Executable`, and `GUI_Executable.spec`. `build` and `GUI_Executable.spec` can be safely deleted.
6. The GUI executable has been created and can now be executed. Congratulations!

