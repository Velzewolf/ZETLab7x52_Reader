CC=gcc
CFLAGS=-Wall -Wextra -O2 -std=c99
TARGET=uart_bridge_daemon
SOURCE=uart_bridge_daemon.c
INSTALL_DIR=/usr/local/bin
CONFIG_DIR=/etc
SERVICE_DIR=/etc/systemd/system

.PHONY: all clean install uninstall enable disable start stop status

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	@echo "Installing UART Bridge Daemon..."
	sudo cp $(TARGET) $(INSTALL_DIR)/
	sudo chmod 755 $(INSTALL_DIR)/$(TARGET)
	sudo cp uart_bridge.conf $(CONFIG_DIR)/
	sudo chmod 644 $(CONFIG_DIR)/uart_bridge.conf
	sudo cp uart-bridge.service $(SERVICE_DIR)/
	sudo chmod 644 $(SERVICE_DIR)/uart-bridge.service
	sudo systemctl daemon-reload
	@echo "Installation complete!"
	@echo "Use 'make enable' to enable the service at boot"
	@echo "Use 'make start' to start the service now"

uninstall:
	@echo "Uninstalling UART Bridge Daemon..."
	sudo systemctl stop uart-bridge 2>/dev/null || true
	sudo systemctl disable uart-bridge 2>/dev/null || true
	sudo rm -f $(INSTALL_DIR)/$(TARGET)
	sudo rm -f $(CONFIG_DIR)/uart_bridge.conf
	sudo rm -f $(SERVICE_DIR)/uart-bridge.service
	sudo systemctl daemon-reload
	@echo "Uninstallation complete!"

enable:
	sudo systemctl enable uart-bridge

disable:
	sudo systemctl disable uart-bridge

start:
	sudo systemctl start uart-bridge

stop:
	sudo systemctl stop uart-bridge

restart:
	sudo systemctl restart uart-bridge

status:
	sudo systemctl status uart-bridge

logs:
	sudo journalctl -u uart-bridge -f