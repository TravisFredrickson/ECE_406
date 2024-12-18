################################################################
# FILE INFO
################################################################

# Author: Travis Fredrickson.
# Date: 2024-12-17.
# Description: A window for a GUI.

################################################################
# INCLUDES
################################################################

#===============================================================
# PyQt.
#===============================================================

from PyQt6.QtCore import *
from PyQt6.QtGui import *
from PyQt6.QtSerialPort import *
from PyQt6.QtWidgets import *

#===============================================================
# Saved for later.
#===============================================================

import serial
import serial.tools.list_ports

################################################################
# WINDOW
################################################################

class window(QMainWindow):

    #===============================================================
    # Initialize.
    #===============================================================

    def __init__(self):
        super().__init__()

        #---------------------------------------------------------------
        # Create a serial object for serial communication
        #---------------------------------------------------------------

        self.serial = serial.Serial()
        self.serial.baudrate = 115200

        #---------------------------------------------------------------
        # Set the window title and icon.
        #---------------------------------------------------------------

        self.setWindowTitle("GUI Window")
        self.setWindowIcon(QIcon("python.png"))

        #---------------------------------------------------------------
        # Create items.
        #---------------------------------------------------------------

        self.QComboBox_port = QComboBox()
        self.QComboBox_port.addItems(["No ports."])

        self.QLabel_left_top = QLabel("Settings")
        self.QLabel_left_bottom = QLabel("Status")
        self.QLabel_right_top = QLabel("Buttons")
        self.QLabel_right_bottom = QLabel("Terminal")

        self.QLabel_command = QLabel("Command")
        self.QLabel_port = QLabel("Port")

        self.QLineEdit_command = QLineEdit()

        self.QPushButton_connect = QPushButton("Connect")
        self.QPushButton_send_command = QPushButton("Send Command")
        self.QPushButton_refresh_ports = QPushButton("Refresh Ports")

        self.QTextEdit_status = QTextEdit("Disconnected.")
        self.QTextEdit_status.setReadOnly(True)
        self.QTextEdit_console = QTextEdit()
        self.QTextEdit_console.setReadOnly(True)

        #---------------------------------------------------------------
        # Create widgets.
        #---------------------------------------------------------------

        self.QWidget_main = QWidget()
        self.QWidget_left_top = QWidget()
        self.QWidget_left_bottom = QWidget()
        self.QWidget_right_top = QWidget()
        self.QWidget_right_bottom = QWidget()

        #---------------------------------------------------------------
        # Create layouts.
        #---------------------------------------------------------------

        self.QLayout_main = QGridLayout()
        self.QLayout_left_top = QGridLayout()
        self.QLayout_left_bottom = QGridLayout()
        self.QLayout_right_top = QGridLayout()
        self.QLayout_right_bottom = QGridLayout()

        #---------------------------------------------------------------
        # Add widgets to layouts.
        #---------------------------------------------------------------

        self.QLayout_main.addWidget(self.QLabel_left_top, 0, 0)
        self.QLayout_main.addWidget(self.QWidget_left_top, 1, 0)
        self.QLayout_main.addWidget(self.QLabel_left_bottom, 2, 0)
        self.QLayout_main.addWidget(self.QWidget_left_bottom, 3, 0)
        self.QLayout_main.addWidget(self.QLabel_right_top, 0, 1)
        self.QLayout_main.addWidget(self.QWidget_right_top, 1, 1)
        self.QLayout_main.addWidget(self.QLabel_right_bottom, 2, 1)
        self.QLayout_main.addWidget(self.QWidget_right_bottom, 3, 1)

        self.QLayout_left_top.addWidget(self.QLabel_port)
        self.QLayout_left_top.addWidget(self.QPushButton_refresh_ports)
        self.QLayout_left_top.addWidget(self.QComboBox_port)
        self.QLayout_left_top.addWidget(self.QPushButton_connect)
        self.QLayout_left_top.addWidget(self.QTextEdit_status)

        self.QLayout_right_bottom.addWidget(self.QTextEdit_console)
        self.QLayout_right_bottom.addWidget(self.QLabel_command)
        self.QLayout_right_bottom.addWidget(self.QLineEdit_command)
        self.QLayout_right_bottom.addWidget(self.QPushButton_send_command)

        #---------------------------------------------------------------
        # Set layouts to widgets.
        #---------------------------------------------------------------

        self.QWidget_main.setLayout(self.QLayout_main)
        self.QWidget_left_top.setLayout(self.QLayout_left_top)
        self.QWidget_left_bottom.setLayout(self.QLayout_left_bottom)
        self.QWidget_right_top.setLayout(self.QLayout_right_top)
        self.QWidget_right_bottom.setLayout(self.QLayout_right_bottom)

        #---------------------------------------------------------------
        # Set central widget.
        #---------------------------------------------------------------

        self.setCentralWidget(self.QWidget_main)

        #---------------------------------------------------------------
        # Styling.
        #---------------------------------------------------------------

        size_1 = 32
        size_2 = 64
        size_3 = 256

        self.QComboBox_port.setFixedHeight(size_1)
        self.QComboBox_port.setCursor(Qt.CursorShape.PointingHandCursor)

        self.QLineEdit_command.setFixedHeight(size_2)

        self.QPushButton_connect.setFixedHeight(size_1)
        self.QPushButton_send_command.setFixedHeight(size_1)
        self.QPushButton_refresh_ports.setFixedHeight(size_1)
        self.QPushButton_send_command.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QPushButton_refresh_ports.setCursor(Qt.CursorShape.PointingHandCursor)

        self.QTextEdit_console.setFixedHeight(size_3)
        self.QTextEdit_status.setFixedHeight(size_2)

        #---------------------------------------------------------------
        # Connect buttons to functions.
        #---------------------------------------------------------------

        self.QLineEdit_command.returnPressed.connect(lambda: self.send_command())

        self.QPushButton_refresh_ports.clicked.connect(lambda: self.refresh_ports())
        self.QPushButton_connect.clicked.connect(lambda: self.connect_to_port())
        self.QPushButton_send_command.clicked.connect(lambda: self.send_command())

    #===============================================================
    # Refresh ports.
    #===============================================================

    def refresh_ports(self):
        # Search for ports.
        ports = serial.tools.list_ports.comports()

        # Update port drop down list in GUI.
        self.QComboBox_port.clear()
        if not ports:
            self.QComboBox_port.addItem("No ports.")
        else:
            for port_name in ports:
                self.QComboBox_port.addItem(str(port_name))

        # Update status text box with number of ports found.
        self.QTextEdit_status.clear()
        self.QTextEdit_status.insertPlainText(f"Found {len(ports)} ports.")

    #===============================================================
    # Connect to port.
    #===============================================================

    def connect_to_port(self):
        # Get port selected by user.
        selected_port_name = self.QComboBox_port.currentText()
        if selected_port_name == "No ports.":
            self.QTextEdit_status.clear()
            self.QTextEdit_status.insertPlainText("No ports connected.")
            return

        # Check if selected port is still available.
        self.refresh_ports()
        selected_port_index = self.QComboBox_port.findText(selected_port_name)
        if selected_port_index == -1:
            self.QTextEdit_status.clear()
            self.QTextEdit_status.insertPlainText(f"{selected_port_name} not connected.")
            return
        else:
            self.QComboBox_port.setCurrentIndex(selected_port_index)
            self.QTextEdit_status.clear()
            self.QTextEdit_status.insertPlainText(f"Connecting to port \"{selected_port_name}\".")

        # Connect to device.
        # Not sure if this "searching for port" loop is the best way.
        # Can we not just find the port directly?
        ports = serial.tools.list_ports.comports()
        for port in ports:
            # print("p = ", port, ".\n")
            if selected_port_name in port.description:
                self.serial.close()
                self.serial.port = str(port.device)
                self.serial.open()

        self.QTextEdit_status.clear()
        self.QTextEdit_status.insertPlainText(f"Connecting to port \"{selected_port_name}\"...")

        # Sleep to let device settle.
        # self.sleep_two_seconds()

        self.QTextEdit_status.clear()
        self.QTextEdit_status.insertPlainText(f"Connected to port \"{selected_port_name}\".")

    #===============================================================
    # Sleep for two seconds.
    #===============================================================

    def sleep_two_seconds(self):
        self.QWidget_main.setEnabled(False)
        QTimer.singleShot(2000, lambda: self.QWidget_main.setDisabled(False))

    #===============================================================
    # Send command.
    #===============================================================

    def send_command(self):
        # Get command.
        command = self.QLineEdit_command.text()
        self.QLineEdit_command.clear()

        # Check if device is connected.
        try:
            self.serial.inWaiting()
        except:
            self.QTextEdit_status.clear()
            self.QTextEdit_status.insertPlainText("No ports connected.")
            return

        # Check if command is empty.
        if command == "":
            self.QTextEdit_status.clear()
            self.QTextEdit_status.insertPlainText("Command is empty.")
            return

        # Disable GUI.
        self.QWidget_main.setEnabled(False)
        self.repaint()
        
        # Send command.
        self.QTextEdit_console.insertPlainText(f"{command}\n")
        self.repaint()
        self.serial.write(command.encode("utf-8"))

        # Re-enable GUI.
        self.QWidget_main.setDisabled(False)
