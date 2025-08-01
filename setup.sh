#!/bin/bash

# UART Bridge Daemon Setup Script for Raspberry Pi 5 (Debian 12)
# This script configures the Raspberry Pi for UART communication and installs the daemon

set -e

echo "UART Bridge Daemon Setup for Raspberry Pi 5"
echo "============================================"

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    echo "Error: This script must be run as root (use sudo)"
    exit 1
fi

# Check if running on Raspberry Pi
if ! grep -q "Raspberry Pi" /proc/cpuinfo; then
    echo "Warning: This script is designed for Raspberry Pi. Continue anyway? (y/N)"
    read -r response
    if [[ ! "$response" =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

echo "Step 1: Installing build dependencies..."
apt-get update
apt-get install -y build-essential git

echo "Step 2: Configuring Raspberry Pi UART..."

# Backup original config.txt
if [ ! -f /boot/firmware/config.txt.backup ]; then
    cp /boot/firmware/config.txt /boot/firmware/config.txt.backup
    echo "Backed up original config.txt"
fi

# Check if UART configuration already exists
if ! grep -q "enable_uart=1" /boot/firmware/config.txt; then
    echo "Adding UART configuration to /boot/firmware/config.txt..."
    cat >> /boot/firmware/config.txt << EOF

# UART Bridge Daemon Configuration
enable_uart=1
dtoverlay=disable-bt
EOF
else
    echo "UART configuration already present in config.txt"
fi

# Disable serial console on ttyAMA0
echo "Step 3: Configuring serial console..."
if [ -f /boot/firmware/cmdline.txt ]; then
    # Remove console=serial0,115200 if present
    sed -i 's/console=serial0,[0-9]*\s*//g' /boot/firmware/cmdline.txt
    echo "Removed serial console from cmdline.txt"
fi

# Configure systemd to not use the serial console
systemctl mask serial-getty@ttyAMA0.service 2>/dev/null || true

echo "Step 4: Setting up user permissions..."
# Add current user to dialout group for UART access
if [ -n "$SUDO_USER" ]; then
    usermod -a -G dialout "$SUDO_USER"
    echo "Added $SUDO_USER to dialout group"
fi

echo "Step 5: Building and installing daemon..."
make clean
make all
make install

echo "Step 6: Configuring daemon..."
# Check if USB UART device exists and update config
if [ -e /dev/ttyUSB0 ]; then
    echo "Found USB UART at /dev/ttyUSB0"
elif [ -e /dev/ttyACM0 ]; then
    echo "Found USB UART at /dev/ttyACM0, updating configuration..."
    sed -i 's|usb_uart_device=/dev/ttyUSB0|usb_uart_device=/dev/ttyACM0|' /etc/uart_bridge.conf
else
    echo "Warning: No USB UART device found. Please connect your USB UART device and update /etc/uart_bridge.conf"
fi

echo "Step 7: Testing UART devices..."
if [ -e /dev/ttyAMA0 ]; then
    echo "✓ Hardware UART (/dev/ttyAMA0) is available"
else
    echo "✗ Hardware UART (/dev/ttyAMA0) not found. Please check Raspberry Pi configuration."
fi

echo ""
echo "Installation completed successfully!"
echo ""
echo "IMPORTANT: A reboot is required for UART changes to take effect."
echo ""
echo "After reboot, you can:"
echo "  - Enable daemon at boot: make enable"
echo "  - Start daemon now: make start"
echo "  - Check daemon status: make status"
echo "  - View daemon logs: make logs"
echo "  - Edit configuration: sudo nano /etc/uart_bridge.conf"
echo ""
echo "Configuration file location: /etc/uart_bridge.conf"
echo "Service logs: journalctl -u uart-bridge"
echo ""
echo "Reboot now? (y/N)"
read -r response
if [[ "$response" =~ ^[Yy]$ ]]; then
    echo "Rebooting..."
    reboot
fi