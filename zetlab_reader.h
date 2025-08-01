#ifndef ZETLAB_READER_H
#define ZETLAB_READER_H

#include <windows.h>

// Device structure
typedef struct {
    unsigned char channel;
    unsigned long baud_rate;
    unsigned char parity;
} Device;

// Measurement structure
typedef struct {
    float x;
    float y;
    float z;
} Measure;

// COM port structure
typedef struct {
    HANDLE com_port;
    char com_name[20];
    Device* connected_device;
} COM;

// Function prototypes
void init_com(COM* com);
int check_com_ports(char ports[][20], int max_ports);
int open_com_port(COM* com, const char* port_name);
int close_com_port(COM* com);
int setup_dcb(COM* com, DWORD baud_rate, BYTE parity);
int scan_devices(COM* com, Device* devices, int max_devices);
Measure get_measure(COM* com, Device device);
float get_axis_value(unsigned char* data, int length, DWORD offset);
int stream_data(COM* com, unsigned char* send_data, int send_length, 
                unsigned char* recv_data, int recv_length);
double get_current_time(void);

// Constants
extern const DWORD BAUD_RATES[];
extern const unsigned short BAUD_RATE_COUNT;
extern const BYTE PARITIES[];
extern const unsigned short PARITY_COUNT;

#endif // ZETLAB_READER_H