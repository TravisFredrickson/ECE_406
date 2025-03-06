################################################################
# FILE INFO
################################################################

# Author: Travis Fredrickson.
# Date: 2025-02-27.
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
        # Set window properties.
        #---------------------------------------------------------------

        self.resize(1024, 1024)
        self.setWindowTitle("GUI Window")
        self.setWindowIcon(QIcon("./sources/python.png"))

        #---------------------------------------------------------------
        # Settings widget.
        #---------------------------------------------------------------

        self.QPushButton_refresh_ports = QPushButton("Refresh Ports")
        self.QLabel_port_name = QLabel("Port Name")
        self.QComboBox_port_names = QComboBox()
        self.QLabel_baud_rate = QLabel("Baud Rate")
        self.QComboBox_baud_rates = QComboBox()
        self.QPushButton_connect = QPushButton("Connect")

        self.QLayout_settings = QGridLayout()
        self.QLayout_settings.addWidget(self.QPushButton_refresh_ports)
        self.QLayout_settings.addWidget(self.QLabel_port_name)
        self.QLayout_settings.addWidget(self.QComboBox_port_names)
        self.QLayout_settings.addWidget(self.QLabel_baud_rate)
        self.QLayout_settings.addWidget(self.QComboBox_baud_rates)
        self.QLayout_settings.addWidget(self.QPushButton_connect)

        self.QWidget_settings = QWidget()
        self.QWidget_settings.setLayout(self.QLayout_settings)

        #---------------------------------------------------------------
        # Commands widget.
        #---------------------------------------------------------------

        self.QLabel_leader_commands = QLabel("Leader Commands")
        self.QPushButton_leader_red_task = QPushButton("Leader Red Task")
        self.QPushButton_leader_yellow_task = QPushButton("Leader Yellow Task")
        self.QPushButton_leader_green_task = QPushButton("Leader Green Task")
        self.QLabel_follower_commands = QLabel("Follower Commands")
        self.QPushButton_follower_red_task = QPushButton("Follower Red Task")
        self.QPushButton_follower_yellow_task = QPushButton("Follower Yellow Task")
        self.QPushButton_follower_green_task = QPushButton("Follower Green Task")
        self.QLabel_custom_command = QLabel("Custom Command")
        self.QLineEdit_custom_command = QLineEdit()
        self.QPushButton_custom_command = QPushButton("Send Custom Command")

        self.QLayout_commands = QGridLayout()
        self.QLayout_commands.addWidget(self.QLabel_leader_commands, 0, 0)
        self.QLayout_commands.addWidget(self.QPushButton_leader_red_task, 1, 0)
        self.QLayout_commands.addWidget(self.QPushButton_leader_yellow_task, 2, 0)
        self.QLayout_commands.addWidget(self.QPushButton_leader_green_task, 3, 0)
        self.QLayout_commands.addWidget(self.QLabel_follower_commands, 0, 1)
        self.QLayout_commands.addWidget(self.QPushButton_follower_red_task, 1, 1)
        self.QLayout_commands.addWidget(self.QPushButton_follower_yellow_task, 2, 1)
        self.QLayout_commands.addWidget(self.QPushButton_follower_green_task, 3, 1)
        self.QLayout_commands.addWidget(self.QLabel_custom_command, 4, 0, 1, 2)
        self.QLayout_commands.addWidget(self.QLineEdit_custom_command, 5, 0, 1, 2)
        self.QLayout_commands.addWidget(self.QPushButton_custom_command, 6, 0, 1, 2)

        self.QWidget_commands = QWidget()
        self.QWidget_commands.setLayout(self.QLayout_commands)

        #---------------------------------------------------------------
        # Terminal widget.
        #---------------------------------------------------------------

        self.QTextEdit_terminal = QTextEdit()

        self.QLayout_terminal = QGridLayout()
        self.QLayout_terminal.addWidget(self.QTextEdit_terminal)

        self.QWidget_terminal = QWidget()
        self.QWidget_terminal.setLayout(self.QLayout_terminal)

        #---------------------------------------------------------------
        # The central widget.
        #---------------------------------------------------------------

        self.QLabel_settings = QLabel("Settings")
        self.QLabel_commands = QLabel("Commands")
        self.QLabel_terminal = QLabel("Terminal")

        self.QLayout_central = QGridLayout()
        self.QLayout_central.addWidget(self.QLabel_settings, 0, 0)
        self.QLayout_central.addWidget(self.QWidget_settings, 1, 0)
        self.QLayout_central.addWidget(self.QLabel_commands, 2, 0)
        self.QLayout_central.addWidget(self.QWidget_commands, 3, 0)
        self.QLayout_central.addWidget(self.QLabel_terminal, 4, 0)
        self.QLayout_central.addWidget(self.QWidget_terminal, 5, 0)

        self.QWidget_central = QWidget()
        self.QWidget_central.setLayout(self.QLayout_central)

        self.setCentralWidget(self.QWidget_central)

        #---------------------------------------------------------------
        # Styling.
        #---------------------------------------------------------------

        size_1 = 32
        size_2 = 64
        size_3 = 256

        self.QPushButton_refresh_ports.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QPushButton_refresh_ports.setFixedHeight(size_1)
        self.QComboBox_port_names.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QComboBox_port_names.setFixedHeight(size_1)
        self.QComboBox_baud_rates.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QComboBox_baud_rates.setFixedHeight(size_1)
        self.QPushButton_connect.setFixedHeight(size_1)
        self.QPushButton_connect.setCursor(Qt.CursorShape.PointingHandCursor)

        self.QPushButton_leader_red_task.setFixedHeight(size_1)
        self.QPushButton_leader_red_task.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QPushButton_leader_yellow_task.setFixedHeight(size_1)
        self.QPushButton_leader_yellow_task.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QPushButton_leader_green_task.setFixedHeight(size_1)
        self.QPushButton_leader_green_task.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QPushButton_follower_red_task.setFixedHeight(size_1)
        self.QPushButton_follower_red_task.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QPushButton_follower_yellow_task.setFixedHeight(size_1)
        self.QPushButton_follower_yellow_task.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QPushButton_follower_green_task.setFixedHeight(size_1)
        self.QPushButton_follower_green_task.setCursor(Qt.CursorShape.PointingHandCursor)

        self.QTextEdit_terminal.setMinimumHeight(size_3)
        self.QTextEdit_terminal.setReadOnly(True)
        self.QLineEdit_custom_command.setFixedHeight(size_2)
        self.QPushButton_custom_command.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QPushButton_custom_command.setFixedHeight(size_1)

        #---------------------------------------------------------------
        # Connect buttons to functions.
        #
        # Use lambda functions if variables need to be passed.
        #---------------------------------------------------------------

        self.QLineEdit_custom_command.returnPressed.connect(self.send_command)

        self.QPushButton_connect.clicked.connect(self.connect_to_port)
        self.QPushButton_refresh_ports.clicked.connect(self.refresh_ports)
        self.QPushButton_custom_command.clicked.connect(lambda: self.send_command(self.QLineEdit_custom_command.text()))

        self.QPushButton_leader_red_task.clicked.connect(lambda: self.send_command("leader_red_task"))
        self.QPushButton_leader_yellow_task.clicked.connect(lambda: self.send_command("leader_yellow_task"))
        self.QPushButton_leader_green_task.clicked.connect(lambda: self.send_command("leader_green_task"))
        self.QPushButton_follower_red_task.clicked.connect(lambda: self.send_command("follower_red_task"))
        self.QPushButton_follower_yellow_task.clicked.connect(lambda: self.send_command("follower_yellow_task"))
        self.QPushButton_follower_green_task.clicked.connect(lambda: self.send_command("follower_green_task"))

        #---------------------------------------------------------------
        # Create a serial port.
        #---------------------------------------------------------------

        self.serial_port = QSerialPort()

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

    def send_command(self, command):
        # Check if port is still open.
        if not self.serial_port.isOpen():
            self.QTextEdit_terminal.insertPlainText("GUI: No ports connected.\n")
            return

        # Check if command is empty.
        if command == "":
            self.QTextEdit_terminal.insertPlainText("GUI: Command is empty.\n")
            return
        
        # Send command.
        self.QTextEdit_terminal.insertPlainText(f"GUI: Sending command \"{command}\".\n")
        self.serial_port.write(command.encode("utf-8"))
