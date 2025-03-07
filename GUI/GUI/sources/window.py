################################################################
# FILE INFO
################################################################

# Author: Travis Fredrickson.
# Date: 2025-03-07.
# Description: A window for a GUI.

################################################################
# INCLUDES
################################################################

from PyQt6.QtCore import *
from PyQt6.QtGui import *
from PyQt6.QtSerialPort import *
from PyQt6.QtWidgets import *

################################################################
# GLOBAL VARIABLES
################################################################

size_1 = 32
size_2 = 64
size_3 = 256

################################################################
# WINDOW
################################################################

class window(QMainWindow):
    #===============================================================
    # __init__()
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
        # Port widget.
        #---------------------------------------------------------------

        # Create items.
        self.QPushButton_refresh_ports = QPushButton("Refresh Ports")
        self.QLabel_port_name = QLabel("Port Name")
        self.QComboBox_port_names = QComboBox()
        self.QLabel_baud_rate = QLabel("Baud Rate")
        self.QComboBox_baud_rates = QComboBox()
        self.QPushButton_connect = QPushButton("Connect")

        # Create layout.
        self.QLayout_port = QGridLayout()
        self.QLayout_port.addWidget(self.QPushButton_refresh_ports)
        self.QLayout_port.addWidget(self.QLabel_port_name)
        self.QLayout_port.addWidget(self.QComboBox_port_names)
        self.QLayout_port.addWidget(self.QLabel_baud_rate)
        self.QLayout_port.addWidget(self.QComboBox_baud_rates)
        self.QLayout_port.addWidget(self.QPushButton_connect)

        # Create widget.
        self.QWidget_port = QWidget()
        self.QWidget_port.setLayout(self.QLayout_port)
        self.QWidget_port.setProperty("css_class", "QWidget_large")

        # Style.
        self.QPushButton_refresh_ports.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QPushButton_refresh_ports.setFixedHeight(size_1)
        self.QComboBox_port_names.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QComboBox_port_names.setFixedHeight(size_1)
        self.QComboBox_baud_rates.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QComboBox_baud_rates.setFixedHeight(size_1)
        self.QPushButton_connect.setFixedHeight(size_1)
        self.QPushButton_connect.setCursor(Qt.CursorShape.PointingHandCursor)

        # Connect items to functions.
        self.QPushButton_refresh_ports.clicked.connect(self.refresh_ports)
        self.QPushButton_connect.clicked.connect(self.connect_to_port)

        #---------------------------------------------------------------
        # Commands widget.
        #---------------------------------------------------------------

        # Create items.
        self.QLabel_leader_commands = QLabel("Leader Commands")
        self.QPushButton_leader_red_task = QPushButton("Leader Red Task")
        self.QPushButton_leader_yellow_task = QPushButton("Leader Yellow Task")
        self.QPushButton_leader_green_task = QPushButton("Leader Green Task")
        self.QLabel_follower_commands = QLabel("Follower Commands")
        self.QPushButton_follower_toggle_led = QPushButton("Follower Toggle LED")
        self.QLabel_custom_command = QLabel("Custom Command")
        self.QLineEdit_custom_command = QLineEdit()
        self.QPushButton_custom_command = QPushButton("Send Custom Command")
        self.QCheckBox_clear_on_send = QCheckBox("Clear on Send")

        # Create layout.
        self.QLayout_commands = QGridLayout()
        self.QLayout_commands.addWidget(self.QLabel_leader_commands, 0, 0)
        self.QLayout_commands.addWidget(self.QPushButton_leader_red_task, 1, 0)
        self.QLayout_commands.addWidget(self.QPushButton_leader_yellow_task, 2, 0)
        self.QLayout_commands.addWidget(self.QPushButton_leader_green_task, 3, 0)
        self.QLayout_commands.addWidget(self.QLabel_follower_commands, 0, 1)
        self.QLayout_commands.addWidget(self.QPushButton_follower_toggle_led, 1, 1)
        self.QLayout_commands.addWidget(self.QLabel_custom_command, 4, 0, 1, 2)
        self.QLayout_commands.addWidget(self.QLineEdit_custom_command, 5, 0, 1, 2)
        self.QLayout_commands.addWidget(self.QPushButton_custom_command, 6, 0, 1, 2)
        self.QLayout_commands.addWidget(self.QCheckBox_clear_on_send, 7, 0, 1, 2)

        # Create widget.
        self.QWidget_commands = QWidget()
        self.QWidget_commands.setLayout(self.QLayout_commands)
        self.QWidget_commands.setProperty("css_class", "QWidget_large")

        # Style.
        self.QPushButton_leader_red_task.setFixedHeight(size_1)
        self.QPushButton_leader_red_task.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QPushButton_leader_yellow_task.setFixedHeight(size_1)
        self.QPushButton_leader_yellow_task.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QPushButton_leader_green_task.setFixedHeight(size_1)
        self.QPushButton_leader_green_task.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QPushButton_follower_toggle_led.setFixedHeight(size_1)
        self.QPushButton_follower_toggle_led.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QLineEdit_custom_command.setFixedHeight(size_1)
        self.QLineEdit_custom_command.setPlaceholderText("Enter custom command here...\n")
        self.QPushButton_custom_command.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QPushButton_custom_command.setFixedHeight(size_1)
        self.QCheckBox_clear_on_send.setCursor(Qt.CursorShape.PointingHandCursor)

        # Connect items to functions.
        self.QPushButton_leader_red_task.clicked.connect(lambda: self.send_command("leader_red_task"))
        self.QPushButton_leader_yellow_task.clicked.connect(lambda: self.send_command("leader_yellow_task"))
        self.QPushButton_leader_green_task.clicked.connect(lambda: self.send_command("leader_green_task"))
        self.QPushButton_follower_toggle_led.clicked.connect(lambda: self.send_command("follower_toggle_led"))
        self.QLineEdit_custom_command.returnPressed.connect(lambda: self.send_custom_command(self.QLineEdit_custom_command.text()))
        self.QPushButton_custom_command.clicked.connect(lambda: self.send_custom_command(self.QLineEdit_custom_command.text()))

        #---------------------------------------------------------------
        # Terminal widget.
        #---------------------------------------------------------------

        # Create items.
        self.QTextEdit_terminal = QTextEdit()
        self.QPushButton_clear_terminal = QPushButton("Clear Terminal")
        self.QCheckBox_auto_scroll = QCheckBox("Auto-Scroll")
        self.QCheckBox_word_wrap = QCheckBox("Word Wrap")

        # Create layout.
        self.QLayout_terminal = QGridLayout()
        self.QLayout_terminal.addWidget(self.QTextEdit_terminal)
        self.QLayout_terminal.addWidget(self.QPushButton_clear_terminal)
        self.QLayout_terminal.addWidget(self.QCheckBox_auto_scroll)
        self.QLayout_terminal.addWidget(self.QCheckBox_word_wrap)

        # Create widget.
        self.QWidget_terminal = QWidget()
        self.QWidget_terminal.setLayout(self.QLayout_terminal)
        self.QWidget_terminal.setProperty("css_class", "QWidget_large")

        # Style.
        self.QTextEdit_terminal.setMinimumHeight(size_3)
        self.QTextEdit_terminal.setReadOnly(True)
        self.QPushButton_clear_terminal.setFixedHeight(size_1)
        self.QPushButton_clear_terminal.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QCheckBox_auto_scroll.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QCheckBox_word_wrap.setCursor(Qt.CursorShape.PointingHandCursor)

        # Connect items to functions.
        self.QPushButton_clear_terminal.clicked.connect(self.clear_terminal)
        self.QCheckBox_word_wrap.toggled.connect(self.toggle_word_wrap)

        #---------------------------------------------------------------
        # The central widget.
        #---------------------------------------------------------------

        # Create items.
        self.QLabel_port = QLabel("Port")
        self.QLabel_port.setProperty("css_class", "QLabel_large")
        self.QLabel_commands = QLabel("Commands")
        self.QLabel_commands.setProperty("css_class", "QLabel_large")
        self.QLabel_terminal = QLabel("Terminal")
        self.QLabel_terminal.setProperty("css_class", "QLabel_large")

        # Create layout.
        self.QLayout_central = QGridLayout()
        self.QLayout_central.addWidget(self.QLabel_port, 0, 0)
        self.QLayout_central.addWidget(self.QWidget_port, 1, 0)
        self.QLayout_central.addWidget(self.QLabel_commands, 2, 0)
        self.QLayout_central.addWidget(self.QWidget_commands, 3, 0)
        self.QLayout_central.addWidget(self.QLabel_terminal, 4, 0)
        self.QLayout_central.addWidget(self.QWidget_terminal, 5, 0)

        # Create widget.
        self.QWidget_central = QWidget()
        self.QWidget_central.setLayout(self.QLayout_central)
        self.QWidget_central.setProperty("css_class", "QWidget_central")
        self.setCentralWidget(self.QWidget_central)

        #---------------------------------------------------------------
        # Create a serial port.
        #---------------------------------------------------------------

        self.serial_port = QSerialPort()

        #---------------------------------------------------------------
        # Initialize states.
        #---------------------------------------------------------------

        # Reefrech ports.
        self.refresh_ports()

        # Get the baud rates and set it to `115200` if available because
        # it is very common.
        baud_rates = [str(baud_rate) for baud_rate in QSerialPortInfo.standardBaudRates()]
        self.QComboBox_baud_rates.addItems(baud_rates)
        value_to_select = "115200"
        index = self.QComboBox_baud_rates.findText(value_to_select)
        if index != -1:
            self.QComboBox_baud_rates.setCurrentIndex(index)

        # Check some boxes.
        self.QCheckBox_clear_on_send.setCheckState(Qt.CheckState.Checked)
        self.QCheckBox_auto_scroll.setCheckState(Qt.CheckState.Checked)
        self.QCheckBox_word_wrap.setCheckState(Qt.CheckState.Checked)

    #===============================================================
    # refresh_ports()
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
            self.QComboBox_port_names.addItem("No ports.\n")

        # Update status text box with number of ports found.
        self.insert_into_terminal(f"GUI: Found {len(available_ports)} ports.\n")

    #===============================================================
    # connect_to_port()
    #===============================================================

    def connect_to_port(self):
        # Set port properties.
        port_name = self.QComboBox_port_names.currentText()
        baud_rate = int(self.QComboBox_baud_rates.currentText())
        self.serial_port.setPortName(port_name)
        self.serial_port.setBaudRate(baud_rate)

        # If connection succeeds.
        if self.serial_port.open(QIODeviceBase.OpenModeFlag.ReadWrite):
            # Change button from connect to disconnect functionality.
            self.QPushButton_connect.clicked.disconnect()
            self.QPushButton_connect.clicked.connect(self.disconnect_from_port)
            self.QPushButton_connect.setText("Disconnect")

            # Disable some GUI items.
            self.QPushButton_refresh_ports.setDisabled(True)
            self.QComboBox_port_names.setDisabled(True)
            self.QComboBox_baud_rates.setDisabled(True)

            # Do some stuff.
            self.insert_into_terminal(f"GUI: Connected to port \"{port_name}\".\n")
            self.timer = QTimer(self)
            self.timer.timeout.connect(self.read_from_port)
            self.timer.start(10)
        # If connection failed.
        else:
            error_message = self.serial_port.errorString()
            self.insert_into_terminal(f"GUI: Failed to connect to port \"{port_name}\": {error_message}.\n")

    #===============================================================
    # disconnect_from_port()
    #===============================================================

    def disconnect_from_port(self):
        # Do some stuff.
        self.timer.stop()
        self.serial_port.close()
        port_name = self.serial_port.portName()
        self.insert_into_terminal(f"GUI: Disonnected from port \"{port_name}\".\n")

        # Change button from disconnect to connect functionality.
        self.QPushButton_connect.clicked.disconnect()
        self.QPushButton_connect.clicked.connect(self.connect_to_port)
        self.QPushButton_connect.setText("Connect")

        # Enable some GUI items.
        self.QPushButton_refresh_ports.setEnabled(True)
        self.QComboBox_port_names.setEnabled(True)
        self.QComboBox_baud_rates.setEnabled(True)

    #===============================================================
    # read_from_port()
    #===============================================================

    def read_from_port(self):
        if self.serial_port.canReadLine():
            data = self.serial_port.readLine().data().decode()
            port_name = self.serial_port.portName()
            self.insert_into_terminal(f"{port_name}: {data}")

    #===============================================================
    # send_command()
    #===============================================================

    def send_command(self, command):
        # Check if port is still open.
        if not self.serial_port.isOpen():
            self.insert_into_terminal("GUI: No ports connected.\n")
            return

        # Check if command is empty.
        if command == "":
            self.insert_into_terminal("GUI: Command is empty.\n")
            return
        
        # Send command.
        self.insert_into_terminal(f"GUI: Sending command \"{command}\".\n")
        self.serial_port.write(command.encode("utf-8"))

    #===============================================================
    # send_custom_command()
    #===============================================================

    def send_custom_command(self, command):
        # Check if port is still open.
        if not self.serial_port.isOpen():
            self.insert_into_terminal("GUI: No ports connected.\n")
            return

        # Check if command is empty.
        if command == "":
            self.insert_into_terminal("GUI: Command is empty.\n")
            return
        
        # Send command.
        self.insert_into_terminal(f"GUI: Sending command \"{command}\".\n")
        self.serial_port.write(command.encode("utf-8"))

        # Clear custom command.
        if self.QCheckBox_clear_on_send.isChecked():
            self.QLineEdit_custom_command.clear()

    #===============================================================
    # insert_into_terminal()
    #===============================================================

    def insert_into_terminal(self, text):
        # Save the current scrollbar positions.
        vertical_scrollbar = self.QTextEdit_terminal.verticalScrollBar()
        vertical_scrollbar_starting_position = vertical_scrollbar.value()
        horizontal_scrollbar = self.QTextEdit_terminal.horizontalScrollBar()
        horizontal_scrollbar_starting_position = horizontal_scrollbar.value()

        # Move the cursor to the end and insert text.
        self.QTextEdit_terminal.moveCursor(QTextCursor.MoveOperation.End)
        self.QTextEdit_terminal.insertPlainText(text)
        self.QTextEdit_terminal.moveCursor(QTextCursor.MoveOperation.End)

        # Do auto-scroll.
        if self.QCheckBox_auto_scroll.isChecked():
            # Move the scrollbars to the bottom-left.
            vertical_scrollbar.setValue(vertical_scrollbar.maximum())
            horizontal_scrollbar.setValue(horizontal_scrollbar.minimum())
        # Do not auto-scroll.
        else:
            # Restore the scrollbars to their starting positions.
            vertical_scrollbar.setValue(vertical_scrollbar_starting_position)
            horizontal_scrollbar.setValue(horizontal_scrollbar_starting_position)

    #===============================================================
    # clear_terminal()
    #===============================================================

    def clear_terminal(self, text):
        self.QTextEdit_terminal.clear()

    #===============================================================
    # toggle_word_wrap()
    #===============================================================
    
    def toggle_word_wrap(self):
        # Do word wrap.
        if self.QCheckBox_word_wrap.isChecked():
            self.QTextEdit_terminal.setWordWrapMode(QTextOption.WrapMode.WordWrap)
        # Do not word wrap.
        else:
            self.QTextEdit_terminal.setWordWrapMode(QTextOption.WrapMode.NoWrap)
