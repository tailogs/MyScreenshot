# MyScreenshot

## Overview

**MyScreenshot** is a lightweight Windows application that allows users to capture screenshots and copy them directly to the clipboard using a hotkey. The application runs in the system tray, ensuring it remains unobtrusive while providing quick access to its functionality.

## Features

- **Screenshot Capture**: Capture the entire screen and copy it to the clipboard.
- **Hotkey Support**: Use `Ctrl + Shift + S` to take a screenshot.
- **System Tray Icon**: The application runs in the system tray for easy access.
- **Exit Option**: Right-click the tray icon to exit the application.

## Requirements

- Windows Operating System
- GCC (MinGW or similar) for compilation
- GDI+ library (included with Windows)

## Installation and Compilation

1. Clone or download the repository.
2. Ensure you have GCC and `windres` installed on your system.
3. Navigate to the project folder and compile the program using the provided `Makefile`:
   ```bash
   make
   ```
4. Run `MyScreenshot.exe` to start the application.

To clean up compiled files, run:
```bash
make clean
```

## Usage

1. Launch `MyScreenshot.exe`. The application will minimize to the system tray.
2. Press `Ctrl + Shift + S` to take a screenshot of your entire screen. The screenshot will be copied to your clipboard.
3. To exit, right-click on the tray icon and select "Exit."

## License

This project is open-source and available under the [MIT License](LICENSE).
