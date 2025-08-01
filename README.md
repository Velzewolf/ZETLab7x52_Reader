# UART Bridge Daemon for Raspberry Pi 5

A robust daemon service that bridges data from a USB UART device to the Raspberry Pi 5's hardware UART (TTY/AMA0) on Debian 12.

## Features

- **Bidirectional UART bridging**: Reads data from USB UART and writes to TTY/AMA0
- **Configurable UART settings**: Baud rate, data bits, stop bits, parity
- **Proper daemon implementation**: Fork-based daemonization with PID file management
- **Systemd integration**: Full systemd service with automatic restart
- **Comprehensive logging**: Uses syslog for status and error reporting
- **Signal handling**: Graceful shutdown on SIGTERM/SIGINT
- **Error recovery**: Robust error handling and automatic restart capability

## Prerequisites

- Raspberry Pi 5 running Debian 12
- USB UART device (e.g., USB-to-Serial adapter)
- Root access for installation and configuration

## Quick Start

1. **Clone or download the project files**
2. **Run the automated setup:**
   ```bash
   sudo ./setup.sh
   ```
3. **Reboot the Raspberry Pi** (required for UART configuration changes)
4. **Enable and start the service:**
   ```bash
   make enable
   make start
   ```

## Manual Installation

If you prefer manual installation:

1. **Build the daemon:**
   ```bash
   make all
   ```

2. **Install the daemon:**
   ```bash
   sudo make install
   ```

3. **Configure Raspberry Pi UART** (add to `/boot/firmware/config.txt`):
   ```
   enable_uart=1
   dtoverlay=disable-bt
   ```

4. **Disable serial console** (remove from `/boot/firmware/cmdline.txt`):
   ```
   console=serial0,115200
   ```

5. **Reboot and start the service:**
   ```bash
   sudo reboot
   # After reboot:
   sudo systemctl enable uart-bridge
   sudo systemctl start uart-bridge
   ```

## Configuration

Edit `/etc/uart_bridge.conf` to customize UART settings:

```ini
# USB UART device path
usb_uart_device=/dev/ttyUSB0

# AMA UART device path (Raspberry Pi hardware UART)
ama_uart_device=/dev/ttyAMA0

# Baud rate (9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600)
baud_rate=115200

# Data bits (5, 6, 7, or 8)
data_bits=8

# Stop bits (1 or 2)
stop_bits=1

# Parity (N=None, E=Even, O=Odd)
parity=N
```

After changing configuration, restart the service:
```bash
sudo systemctl restart uart-bridge
```

## Service Management

Use the provided Makefile targets for easy service management:

```bash
# Build the daemon
make all

# Install/uninstall
make install
make uninstall

# Service control
make enable     # Enable at boot
make disable    # Disable at boot
make start      # Start service
make stop       # Stop service
make restart    # Restart service
make status     # Show service status
make logs       # Follow service logs

# Clean build files
make clean
```

## Manual Service Commands

Alternatively, use systemctl directly:

```bash
# Service control
sudo systemctl start uart-bridge
sudo systemctl stop uart-bridge
sudo systemctl restart uart-bridge
sudo systemctl enable uart-bridge
sudo systemctl disable uart-bridge

# Status and logs
sudo systemctl status uart-bridge
sudo journalctl -u uart-bridge -f
```

## Troubleshooting

### 1. Service fails to start

Check the logs:
```bash
sudo journalctl -u uart-bridge -n 50
```

Common issues:
- USB UART device not connected or wrong path in config
- TTY/AMA0 not available (check Raspberry Pi UART configuration)
- Permission issues (ensure daemon runs as root)

### 2. USB UART device not found

List available serial devices:
```bash
ls -la /dev/ttyUSB* /dev/ttyACM*
```

Update the configuration file with the correct device path.

### 3. TTY/AMA0 not available

Ensure these are added to `/boot/firmware/config.txt`:
```
enable_uart=1
dtoverlay=disable-bt
```

And remove serial console from `/boot/firmware/cmdline.txt`.

### 4. Permission denied

Ensure your user is in the `dialout` group:
```bash
sudo usermod -a -G dialout $USER
```

### 5. Test UART communication

Test the hardware UART:
```bash
# Send test data
echo "test" > /dev/ttyAMA0

# Read data (in another terminal)
cat /dev/ttyAMA0
```

## Technical Details

### Daemon Architecture

- **Double fork**: Proper daemon implementation following Unix conventions
- **Signal handling**: Clean shutdown on SIGTERM/SIGINT
- **PID file management**: Prevents multiple instances
- **Syslog integration**: All events logged to system log

### UART Configuration

- **Raw mode**: No input/output processing
- **Non-blocking reads**: Prevents daemon from hanging
- **Configurable timeouts**: 0.1-second read timeout
- **Hardware flow control**: Disabled for maximum compatibility

### Error Handling

- **Graceful degradation**: Continues operation on non-fatal errors
- **Automatic restart**: systemd restarts on failure
- **Resource cleanup**: Proper file descriptor and PID file cleanup
- **Comprehensive logging**: All errors logged with context

## Hardware Connections

### Raspberry Pi 5 GPIO Pins (TTY/AMA0)
- **Pin 8 (GPIO14)**: TX (Transmit)
- **Pin 10 (GPIO15)**: RX (Receive)
- **Pin 6 or 14**: GND (Ground)

### Typical Wiring
```
Raspberry Pi 5    <->    External Device
Pin 8 (TX)        <->    RX
Pin 10 (RX)       <->    TX
Pin 6 (GND)       <->    GND
```

## File Structure

```
.
├── uart_bridge_daemon.c    # Main daemon source code
├── uart-bridge.service     # systemd service file
├── uart_bridge.conf        # Configuration file
├── Makefile               # Build and installation targets
├── setup.sh              # Automated setup script
└── README.md             # This documentation
```

## License

This project is provided as-is for educational and practical use. Modify as needed for your specific requirements.

## Support

For issues and questions:
1. Check the logs: `sudo journalctl -u uart-bridge`
2. Verify UART device availability: `ls -la /dev/tty*`
3. Test hardware connections
4. Review configuration file: `/etc/uart_bridge.conf`