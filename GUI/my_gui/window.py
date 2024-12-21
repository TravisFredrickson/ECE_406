################################################################
# FILE INFO
################################################################

# Author: Travis Fredrickson.
# Date: 2024-12-17.
# Description: A window for a GUI.

################################################################
# INCLUDES
################################################################

from PyQt6.QtCore import *
from PyQt6.QtGui import *
from PyQt6.QtSerialPort import *
from PyQt6.QtWidgets import *

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
        # Create a serial port.
        #---------------------------------------------------------------

        self.serial_port = QSerialPort()

        #---------------------------------------------------------------
        # Set window properties.
        #---------------------------------------------------------------

        self.resize(1024, 1024)
        self.setWindowTitle("GUI Window")
        self.setWindowIcon(QIcon("python.png"))

        #---------------------------------------------------------------
        # Create items.
        #---------------------------------------------------------------

        self.QComboBox_port_names = QComboBox()
        self.QComboBox_baud_rates = QComboBox()

        self.QLabel_left_top = QLabel("Port")
        self.QLabel_left_bottom = QLabel("Status")
        self.QLabel_right_top = QLabel("Buttons")
        self.QLabel_right_bottom = QLabel("Terminal")

        self.QLabel_command = QLabel("Command")
        self.QLabel_baud_rate = QLabel("Baud Rate")
        self.QLabel_port_name = QLabel("Port Name")

        self.QLineEdit_command = QLineEdit()

        self.QPushButton_connect = QPushButton("Connect")
        self.QPushButton_send_command = QPushButton("Send Command")
        self.QPushButton_refresh_ports = QPushButton("Refresh Ports")

        self.QTextEdit_terminal = QTextEdit()
        self.QTextEdit_terminal.setReadOnly(True)

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
        # self.QLayout_main.addWidget(self.QLabel_left_bottom, 2, 0)
        # self.QLayout_main.addWidget(self.QWidget_left_bottom, 3, 0)
        # self.QLayout_main.addWidget(self.QLabel_right_top, 0, 1)
        # self.QLayout_main.addWidget(self.QWidget_right_top, 1, 1)
        self.QLayout_main.addWidget(self.QLabel_right_bottom, 2, 0)
        self.QLayout_main.addWidget(self.QWidget_right_bottom, 3, 0)

        self.QLayout_left_top.addWidget(self.QPushButton_refresh_ports)
        self.QLayout_left_top.addWidget(self.QLabel_port_name)
        self.QLayout_left_top.addWidget(self.QComboBox_port_names)
        self.QLayout_left_top.addWidget(self.QLabel_baud_rate)
        self.QLayout_left_top.addWidget(self.QComboBox_baud_rates)
        self.QLayout_left_top.addWidget(self.QPushButton_connect)

        self.QLayout_right_bottom.addWidget(self.QTextEdit_terminal)
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

        self.QComboBox_port_names.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QComboBox_port_names.setFixedHeight(size_1)
        self.QComboBox_baud_rates.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QComboBox_baud_rates.setFixedHeight(size_1)

        self.QLineEdit_command.setFixedHeight(size_2)

        self.QPushButton_connect.setFixedHeight(size_1)
        self.QPushButton_connect.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QPushButton_refresh_ports.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QPushButton_refresh_ports.setFixedHeight(size_1)
        self.QPushButton_send_command.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QPushButton_send_command.setFixedHeight(size_1)

        self.QTextEdit_terminal.setMinimumHeight(size_3)

        #---------------------------------------------------------------
        # Connect buttons to functions.
        #
        # Use lambda functions if variables need to be passed.
        #---------------------------------------------------------------

        self.QLineEdit_command.returnPressed.connect(self.send_command)

        self.QPushButton_connect.clicked.connect(self.connect_to_port)
        self.QPushButton_refresh_ports.clicked.connect(self.refresh_ports)
        self.QPushButton_send_command.clicked.connect(self.send_command)

        #---------------------------------------------------------------
        # Initialize states.
        #---------------------------------------------------------------

        self.refresh_ports()

        baud_rates = [str(baud_rate) for baud_rate in QSerialPortInfo.standardBaudRates()]
        self.QComboBox_baud_rates.addItems(baud_rates)

    #===============================================================
    # Refresh ports.
    #===============================================================

    def refresh_ports(self):
        # Search for ports.
        available_ports = QSerialPortInfo.availablePorts()

        # Update port drop down list in GUI.
        if available_ports:
            port_names = [(port.portName()) for port in available_ports]
            self.QComboBox_port_names.clear()
            self.QComboBox_port_names.addItems(port_names)
        else:
            self.QComboBox_port_names.clear()
            self.QComboBox_port_names.addItem("No ports.")

        # Update status text box with number of ports found.
        self.QTextEdit_terminal.insertPlainText(f"GUI: Found {len(available_ports)} ports.\n")

    #===============================================================
    # Connect to port.
    #===============================================================

    def connect_to_port(self):

        # Set port properties.
        port_name = self.QComboBox_port_names.currentText()
        baud_rate = int(self.QComboBox_baud_rates.currentText())
        self.serial_port.setPortName(port_name)
        self.serial_port.setBaudRate(baud_rate)

        # Attempt to connect to port.
        # Maybe add a check for if it busy.
        if self.serial_port.open(QIODeviceBase.OpenModeFlag.ReadWrite):
            # Connection successful.
            self.QPushButton_connect.clicked.disconnect()
            self.QPushButton_connect.clicked.connect(self.disconnect_from_port)
            self.QTextEdit_terminal.insertPlainText(f"GUI: Connected to port \"{port_name}\".\n")
            self.QPushButton_connect.setText("Disconnect")
            self.timer = QTimer(self)
            self.timer.timeout.connect(self.read_from_port)
            self.timer.start(10)
        else:
            # Connection failed.
            error_message = self.serial_port.errorString()
            self.QTextEdit_terminal.insertPlainText(f"GUI: Failed to connect to port \"{port_name}\": {error_message}.\n")

    #===============================================================
    # Disconnect from port.
    #===============================================================

    def disconnect_from_port(self):
        self.timer.stop()
        self.serial_port.close()
        self.QPushButton_connect.clicked.disconnect()
        self.QPushButton_connect.clicked.connect(self.connect_to_port)
        port_name = self.serial_port.portName()
        self.QTextEdit_terminal.insertPlainText(f"GUI: Disonnected from port \"{port_name}\".\n")
        self.QPushButton_connect.setText("Connect")

    #===============================================================
    # Read from port.
    #===============================================================

    def read_from_port(self):
        if self.serial_port.canReadLine():
            data = self.serial_port.readLine().data().decode()
            port_name = self.serial_port.portName()
            self.QTextEdit_terminal.insertPlainText(f"{port_name}: {data}")

    #===============================================================
    # Send command.
    #===============================================================

    def send_command(self):
        # Check if port is still open.
        if not self.serial_port.isOpen():
            self.QTextEdit_terminal.insertPlainText("GUI: No ports connected.\n")
            return

        # Get command.
        command = self.QLineEdit_command.text()
        # self.QLineEdit_command.clear()

        # Check if command is empty.
        if command == "":
            self.QTextEdit_terminal.insertPlainText("GUI: Command is empty.\n")
            return
        
        # Send command.
        self.QTextEdit_terminal.insertPlainText(f"GUI: Sending command \"{command}\".\n")
        self.serial_port.write(command.encode("utf-8"))
