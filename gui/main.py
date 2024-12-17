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

if __name__ == '__main__':
    # Print as a test if the program is working.
    print('Booting up GUI program.\n')

    # Create the application.
    app = QApplication(sys.argv)

    # Load CSS stylesheet.
    with open('styles.css', 'r') as file:
        app.setStyleSheet(file.read())

    # Show the window.
    my_window = window()
    my_window.show()

    # Execute the application.
    sys.exit(app.exec())
