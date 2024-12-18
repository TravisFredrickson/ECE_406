import sys
from PyQt6.QtCore import *
from PyQt6.QtWidgets import *
from PyQt6.QtSerialPort import *

class SerialReader(QWidget):
    def __init__(self):
        super().__init__()

        self.serialPort = QSerialPort()

        self.setWindowTitle('Serial Port Reader')
        self.setGeometry(300, 300, 300, 200)

        # Create UI elements
        self.portLabel = QLabel('Port:')
        self.portLineEdit = QLineEdit('/dev/ttyACM0')
        self.baudRateLabel = QLabel('Baud Rate:')
        self.baudRateLineEdit = QLineEdit('115200')
        self.connectButton = QPushButton('Connect')
        self.disconnectButton = QPushButton('Disconnect')
        self.textBox = QTextEdit()

        # Create layout
        vbox = QVBoxLayout()
        vbox.addWidget(self.portLabel)
        vbox.addWidget(self.portLineEdit)
        vbox.addWidget(self.baudRateLabel)
        vbox.addWidget(self.baudRateLineEdit)
        vbox.addWidget(self.connectButton)
        vbox.addWidget(self.disconnectButton)
        vbox.addWidget(self.textBox)
        self.setLayout(vbox)

        # Connect button clicks to functions
        self.connectButton.clicked.connect(self.connectSerialPort)
        self.disconnectButton.clicked.connect(self.disconnectSerialPort)

    def connectSerialPort(self):
        portName = self.portLineEdit.text()
        baudRate = int(self.baudRateLineEdit.text())
        self.serialPort.setPortName(portName)
        self.serialPort.setBaudRate(baudRate)
        self.serialPort.open(QIODeviceBase.OpenModeFlag.ReadOnly)

        self.connectButton.setEnabled(False)
        self.disconnectButton.setEnabled(True)

        self.timer = QTimer(self)
        self.timer.timeout.connect(self.readSerialData)
        self.timer.start(100)

    def disconnectSerialPort(self):
        self.serialPort.close()
        self.connectButton.setEnabled(True)
        self.disconnectButton.setEnabled(False)
        self.timer.stop()

    def readSerialData(self):
        if self.serialPort.canReadLine():
            data = self.serialPort.readLine().data().decode().strip()
            self.textBox.setText(data)

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = SerialReader()
    window.show()
    sys.exit(app.exec())
