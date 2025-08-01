#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "zetlab_reader.h"

// Supported baud rates
const DWORD BAUD_RATES[] = {
    CBR_38400
};
const unsigned short BAUD_RATE_COUNT = sizeof(BAUD_RATES) / sizeof(DWORD);

// Supported parities
const BYTE PARITIES[] = {
    NOPARITY,
    ODDPARITY
};
const unsigned short PARITY_COUNT = sizeof(PARITIES) / sizeof(BYTE);

// Initialize COM structure
void init_com(COM* com) {
    com->com_port = NULL;
    memset(com->com_name, 0, sizeof(com->com_name));
    com->connected_device = NULL;
}

// Check available COM ports
int check_com_ports(char ports[][20], int max_ports) {
    int port_count = 0;
    char port_name[20];
    HANDLE test_handle;
    
    for (int i = 1; i <= 256 && port_count < max_ports; i++) {
        if (i < 10) {
            sprintf(port_name, "COM%d", i);
        } else {
            sprintf(port_name, "\\\\.\\COM%d", i);
        }
        
        test_handle = CreateFile(port_name,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL);
            
        if (test_handle != INVALID_HANDLE_VALUE && GetLastError() != ERROR_ACCESS_DENIED) {
            strcpy(ports[port_count], port_name);
            port_count++;
            CloseHandle(test_handle);
        }
    }
    
    return port_count;
}

// Open COM port
int open_com_port(COM* com, const char* port_name) {
    strcpy(com->com_name, port_name);
    
    com->com_port = CreateFile(port_name,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);
        
    if (com->com_port == INVALID_HANDLE_VALUE) {
        printf("Error opening COM port %s\n", port_name);
        return 0;
    }
    
    return 1;
}

// Close COM port
int close_com_port(COM* com) {
    if (com->com_port != NULL && com->com_port != INVALID_HANDLE_VALUE) {
        CloseHandle(com->com_port);
        com->com_port = NULL;
        return 1;
    }
    return 0;
}

// Setup DCB (Device Control Block)
int setup_dcb(COM* com, DWORD baud_rate, BYTE parity) {
    DCB dcb;
    COMMTIMEOUTS timeouts;
    
    if (!GetCommState(com->com_port, &dcb)) {
        printf("Error getting COM state\n");
        return 0;
    }
    
    dcb.BaudRate = baud_rate;
    dcb.ByteSize = 8;
    dcb.Parity = parity;
    dcb.StopBits = ONESTOPBIT;
    dcb.fBinary = TRUE;
    dcb.fParity = (parity != NOPARITY);
    
    if (!SetCommState(com->com_port, &dcb)) {
        printf("Error setting COM state\n");
        return 0;
    }
    
    // Set timeouts
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    
    if (!SetCommTimeouts(com->com_port, &timeouts)) {
        printf("Error setting COM timeouts\n");
        return 0;
    }
    
    return 1;
}

// Stream data to/from device
int stream_data(COM* com, unsigned char* send_data, int send_length, 
                unsigned char* recv_data, int recv_length) {
    DWORD bytes_written, bytes_read;
    
    // Send data
    if (!WriteFile(com->com_port, send_data, send_length, &bytes_written, NULL)) {
        printf("Error writing to COM port\n");
        return 0;
    }
    
    // Receive data
    if (!ReadFile(com->com_port, recv_data, recv_length, &bytes_read, NULL)) {
        printf("Error reading from COM port\n");
        return 0;
    }
    
    return bytes_read;
}

// Get axis value from received data
float get_axis_value(unsigned char* data, int length, DWORD offset) {
    if (offset + 3 >= (DWORD)length) {
        return 0.0f;
    }
    
    // Convert 4 bytes to float (assuming little-endian)
    float value;
    memcpy(&value, &data[offset], sizeof(float));
    return value;
}

// Get measurement from device
Measure get_measure(COM* com, Device device) {
    Measure measure = {0.0f, 0.0f, 0.0f};
    unsigned char send_cmd[] = {0x01, device.channel, 0x03}; // Example command
    unsigned char recv_buffer[256];
    
    int bytes_received = stream_data(com, send_cmd, sizeof(send_cmd), 
                                   recv_buffer, sizeof(recv_buffer));
    
    if (bytes_received >= 12) { // Expecting at least 12 bytes for 3 floats
        measure.x = get_axis_value(recv_buffer, bytes_received, 0);
        measure.y = get_axis_value(recv_buffer, bytes_received, 4);
        measure.z = get_axis_value(recv_buffer, bytes_received, 8);
    }
    
    return measure;
}

// Scan for connected devices
int scan_devices(COM* com, Device* devices, int max_devices) {
    int device_count = 0;
    
    for (int baud_idx = 0; baud_idx < BAUD_RATE_COUNT && device_count < max_devices; baud_idx++) {
        for (int parity_idx = 0; parity_idx < PARITY_COUNT && device_count < max_devices; parity_idx++) {
            if (!setup_dcb(com, BAUD_RATES[baud_idx], PARITIES[parity_idx])) {
                continue;
            }
            
            // Try to communicate with different channels
            for (unsigned char channel = 1; channel <= 16; channel++) {
                unsigned char test_cmd[] = {0x01, channel, 0x01}; // Test command
                unsigned char response[64];
                
                int bytes_received = stream_data(com, test_cmd, sizeof(test_cmd), 
                                               response, sizeof(response));
                
                if (bytes_received > 0 && response[0] == 0x01) { // Valid response
                    devices[device_count].channel = channel;
                    devices[device_count].baud_rate = BAUD_RATES[baud_idx];
                    devices[device_count].parity = PARITIES[parity_idx];
                    device_count++;
                    
                    printf("Found device: Channel %d, Baud %lu, Parity %d\n", 
                           channel, BAUD_RATES[baud_idx], PARITIES[parity_idx]);
                }
            }
        }
    }
    
    return device_count;
}

// Get current time in seconds
double get_current_time(void) {
    return (double)clock() / CLOCKS_PER_SEC;
}

// Main function - demonstration
int main(void) {
    COM com;
    char available_ports[32][20];
    Device devices[16];
    int port_count, device_count;
    
    printf("ZET Lab 7052 Serial Communication Tool (C Version)\n");
    printf("================================================\n\n");
    
    // Initialize COM
    init_com(&com);
    
    // Check available COM ports
    printf("Scanning for available COM ports...\n");
    port_count = check_com_ports(available_ports, 32);
    
    if (port_count == 0) {
        printf("No COM ports found!\n");
        return 1;
    }
    
    printf("Found %d COM port(s):\n", port_count);
    for (int i = 0; i < port_count; i++) {
        printf("  %s\n", available_ports[i]);
    }
    
    // Try to open the first available port
    if (!open_com_port(&com, available_ports[0])) {
        printf("Failed to open COM port %s\n", available_ports[0]);
        return 1;
    }
    
    printf("\nOpened COM port: %s\n", com.com_name);
    
    // Scan for devices
    printf("Scanning for connected devices...\n");
    device_count = scan_devices(&com, devices, 16);
    
    if (device_count == 0) {
        printf("No devices found!\n");
    } else {
        printf("Found %d device(s)\n\n", device_count);
        
        // Read measurements from first device
        if (device_count > 0) {
            printf("Reading measurements from first device...\n");
            
            for (int i = 0; i < 10; i++) {
                Measure measure = get_measure(&com, devices[0]);
                double timestamp = get_current_time();
                
                printf("Time: %.3f, X: %.3f, Y: %.3f, Z: %.3f\n", 
                       timestamp, measure.x, measure.y, measure.z);
                
                Sleep(100); // Wait 100ms between readings
            }
        }
    }
    
    // Clean up
    close_com_port(&com);
    printf("\nCOM port closed. Exiting.\n");
    
    return 0;
}