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
from PyQt6.QtWidgets import *

#===============================================================
# Saved for later.
#===============================================================

# import serial
# import serial.tools.list_ports
# import re

################################################################
# WINDOW
################################################################

class window(QMainWindow):
    def __init__(self):
        super().__init__()

        #===============================================================
        # GUI.
        #===============================================================

        #---------------------------------------------------------------
        # Set the window title and icon.
        #---------------------------------------------------------------

        self.setWindowTitle('GUI Window')
        self.setWindowIcon(QIcon('python.png'))

        #---------------------------------------------------------------
        # Create GUI items.
        #---------------------------------------------------------------

        self.QComboBox_port = QComboBox()
        self.QComboBox_port.addItems(['No Ports'])

        self.QLabel_left_top = QLabel('Settings')
        self.QLabel_left_bottom = QLabel('System Status')
        self.QLabel_right_top = QLabel('Jog Controller')
        self.QLabel_right_bottom = QLabel('Console Controller')
        self.QLabel_baud = QLabel('Baud')
        self.QLabel_command = QLabel('Command: ')
        self.QLabel_console = QLabel('Console')
        self.QLabel_port = QLabel('Port')
        self.QLabel_stepSize = QLabel('XYZ\nStep\nSize:')
        self.QLabel_x = QLabel('X:')
        self.QLabel_y = QLabel('Y:')
        self.QLabel_z = QLabel('Z:')

        self.QLineEdit_baud = QLineEdit('115200')
        self.QLineEdit_baud.setReadOnly(True)
        self.QLineEdit_command = QLineEdit()
        self.QLineEdit_stepSize = QLineEdit('5')
        self.QLineEdit_x = QLineEdit('0')
        self.QLineEdit_x.setReadOnly(True)
        self.QLineEdit_y = QLineEdit('0')
        self.QLineEdit_y.setReadOnly(True)
        self.QLineEdit_z = QLineEdit('0')
        self.QLineEdit_z.setReadOnly(True)

        self.QPushButton_jog_xpos = QPushButton('X')
        self.QPushButton_jog_xneg = QPushButton('X-')
        self.QPushButton_jog_ypos = QPushButton('Y')
        self.QPushButton_jog_yneg = QPushButton('Y-')
        self.QPushButton_jog_zpos = QPushButton('Z')
        self.QPushButton_jog_zneg = QPushButton('Z-')
        self.QPushButton_connect = QPushButton('Connect')
        self.QPushButton_execute = QPushButton('Execute')
        self.QPushButton_refreshPorts = QPushButton('Refresh Ports')
        self.QPushButton_units = QPushButton('mm')

        self.QTextEdit_status = QTextEdit('Disconnected')
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
        self.QLayout_left_top = QVBoxLayout()
        self.QLayout_left_bottom = QGridLayout()
        self.QLayout_right_top = QGridLayout()
        self.QLayout_right_bottom = QVBoxLayout()

        #---------------------------------------------------------------
        # Add items to layouts.
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
        self.QLayout_left_top.addWidget(self.QPushButton_refreshPorts)
        self.QLayout_left_top.addWidget(self.QComboBox_port)
        self.QLayout_left_top.addWidget(self.QPushButton_connect)
        self.QLayout_left_top.addWidget(self.QLabel_baud)
        self.QLayout_left_top.addWidget(self.QLineEdit_baud)

        self.QLayout_left_bottom.addWidget(self.QTextEdit_status, 0, 0, 1, 2)
        self.QLayout_left_bottom.addWidget(self.QLabel_x, 1, 0)
        self.QLayout_left_bottom.addWidget(self.QLineEdit_x, 1, 1)
        self.QLayout_left_bottom.addWidget(self.QLabel_y, 2, 0)
        self.QLayout_left_bottom.addWidget(self.QLineEdit_y, 2, 1)
        self.QLayout_left_bottom.addWidget(self.QLabel_z, 3, 0)
        self.QLayout_left_bottom.addWidget(self.QLineEdit_z, 3, 1)

        self.QLayout_right_top.addWidget(self.QPushButton_jog_xpos, 0, 2)
        self.QLayout_right_top.addWidget(self.QPushButton_jog_xneg, 0, 0)
        self.QLayout_right_top.addWidget(self.QPushButton_jog_ypos, 0, 1)
        self.QLayout_right_top.addWidget(self.QPushButton_jog_yneg, 1, 1)
        self.QLayout_right_top.addWidget(self.QPushButton_jog_zpos, 1, 0)
        self.QLayout_right_top.addWidget(self.QPushButton_jog_zneg, 1, 2)
        self.QLayout_right_top.addWidget(self.QPushButton_units, 2, 0)
        self.QLayout_right_top.addWidget(self.QLabel_stepSize, 2, 1)
        self.QLayout_right_top.addWidget(self.QLineEdit_stepSize, 2, 2)

        self.QLayout_right_bottom.addWidget(self.QLabel_console)
        self.QLayout_right_bottom.addWidget(self.QTextEdit_console)
        self.QLayout_right_bottom.addWidget(self.QLabel_command)
        self.QLayout_right_bottom.addWidget(self.QLineEdit_command)
        self.QLayout_right_bottom.addWidget(self.QPushButton_execute)

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

        #===============================================================
        # Styling.
        #===============================================================

        size_1 = 32
        size_2 = 64

        self.QComboBox_port.setFixedHeight(size_1)
        self.QComboBox_port.setCursor(Qt.CursorShape.PointingHandCursor)

        self.QLabel_stepSize.setFixedSize(size_2, size_2)

        self.QLineEdit_baud.setFixedHeight(size_1)
        self.QLineEdit_command.setFixedHeight(size_2)
        self.QLineEdit_stepSize.setFixedSize(size_2, size_2)
        self.QLineEdit_x.setFixedHeight(size_2)
        self.QLineEdit_y.setFixedHeight(size_2)
        self.QLineEdit_z.setFixedHeight(size_2)
        self.QLineEdit_stepSize.setAlignment(Qt.AlignmentFlag.AlignHCenter)

        self.QPushButton_jog_xpos.setFixedSize(size_2, size_2)
        self.QPushButton_jog_xneg.setFixedSize(size_2, size_2)
        self.QPushButton_jog_ypos.setFixedSize(size_2, size_2)
        self.QPushButton_jog_yneg.setFixedSize(size_2, size_2)
        self.QPushButton_jog_zpos.setFixedSize(size_2, size_2)
        self.QPushButton_jog_zneg.setFixedSize(size_2, size_2)
        self.QPushButton_connect.setFixedHeight(size_1)
        self.QPushButton_execute.setFixedHeight(size_1)
        self.QPushButton_refreshPorts.setFixedHeight(size_1)
        self.QPushButton_units.setFixedSize(size_2, size_2)
        self.QPushButton_jog_xpos.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QPushButton_jog_xneg.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QPushButton_jog_ypos.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QPushButton_jog_yneg.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QPushButton_jog_zpos.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QPushButton_jog_zneg.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QPushButton_connect.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QPushButton_execute.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QPushButton_refreshPorts.setCursor(Qt.CursorShape.PointingHandCursor)
        self.QPushButton_units.setCursor(Qt.CursorShape.PointingHandCursor)

        self.QTextEdit_console.setMinimumHeight(size_2 * 4)
        self.QTextEdit_status.setFixedHeight(size_2)
