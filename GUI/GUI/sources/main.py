################################################################
# FILE INFO
################################################################

# Author: Travis Fredrickson.
# Date: 2024-12-17.
# Description: Creates a GUI.

################################################################
# INCLUDES
################################################################

#===============================================================
# Standard.
#===============================================================

import sys

#===============================================================
# PyQt.
#===============================================================

# from PyQt6.QtCore import *
# from PyQt6.QtGui import *
from PyQt6.QtWidgets import *

#===============================================================
# User.
#===============================================================

from window import window

################################################################
# MAIN
################################################################

if __name__ == "__main__":
    # Print a message to signal that the program is working.
    print("Booting up GUI program.\n")

    # Create the application.
    app = QApplication(sys.argv)

    # Load CSS stylesheet.
    with open("./sources/styles.css", "r") as file:
        app.setStyleSheet(file.read())

    # Show the `window` on the computer.
    my_window = window()
    my_window.show()

    # Start the application.
    sys.exit(app.exec())
