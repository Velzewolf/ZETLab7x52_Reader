ZET Lab 7052 Serial Communication Tool (C Version)
==================================================

This is a pure C implementation of a serial communication tool for ZET Lab devices.
It provides functionality similar to the original C++ Windows Forms application.

Files:
------
- zetlab_reader.c   - Main C source code
- zetlab_reader.h   - Header file with declarations
- zetlab_reader.exe - Compiled Windows executable
- Makefile          - Build configuration
- README.txt        - This documentation

Features:
---------
1. COM port scanning and detection
2. Device discovery across different baud rates and parity settings
3. Serial communication with measurement devices
4. Real-time data reading (X, Y, Z axis measurements)
5. Timestamp logging

Building:
---------
On Linux with MinGW cross-compiler:
    make

On Windows with GCC:
    gcc -Wall -Wextra -std=c99 -o zetlab_reader.exe zetlab_reader.c -lkernel32

Usage:
------
Run the executable on Windows:
    zetlab_reader.exe

The program will:
1. Scan for available COM ports
2. Attempt to open the first available port
3. Search for connected ZET Lab devices
4. Read measurements from the first found device
5. Display real-time X, Y, Z axis data with timestamps

Protocol Details:
-----------------
- Supported baud rate: 38400 bps
- Supported parity: None, Odd
- Data bits: 8
- Stop bits: 1
- Device channels: 1-16
- Command format: [0x01, channel, command_type]
- Response format: Device-specific binary data

Customization:
--------------
To modify the communication protocol:
1. Edit BAUD_RATES array for different baud rates
2. Edit PARITIES array for different parity settings
3. Modify command format in get_measure() function
4. Adjust data parsing in get_axis_value() function

Notes:
------
- This is designed for Windows COM port communication
- Requires appropriate device drivers to be installed
- Device-specific command protocols may need adjustment
- Error handling includes timeout and communication checks

Troubleshooting:
---------------
- Ensure COM port is not in use by other applications
- Check device drivers are properly installed
- Verify device is powered and connected
- Try different COM ports if auto-detection fails