import sys
import serial
from PyQt6.QtWidgets import QApplication, QMainWindow, QWidget, QVBoxLayout, QPushButton, QLineEdit, QLabel

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setWindowTitle("ESP32 Control")

        layout = QVBoxLayout()

        self.command_input = QLineEdit()
        layout.addWidget(self.command_input)

        self.send_button = QPushButton("Send Command")
        self.send_button.clicked.connect(self.send_command)
        layout.addWidget(self.send_button)

        self.output_label = QLabel()
        layout.addWidget(self.output_label)

        central_widget = QWidget()
        central_widget.setLayout(layout)
        self.setCentralWidget(central_widget)

        self.ser = serial.Serial('/dev/ttyACM0', 115200)

    def send_command(self):
        command = self.command_input.text()
        self.ser.write(command.encode())

        response = self.ser.readline().decode().strip()
        self.output_label.setText(response)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())
