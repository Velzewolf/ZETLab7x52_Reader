#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>

#define DAEMON_NAME "uart_bridge"
#define PID_FILE "/var/run/uart_bridge.pid"
#define DEFAULT_USB_UART "/dev/ttyUSB0"
#define DEFAULT_AMA_UART "/dev/ttyAMA0"
#define BUFFER_SIZE 256
#define CONFIG_FILE "/etc/uart_bridge.conf"

typedef struct {
    char usb_uart_device[256];
    char ama_uart_device[256];
    int baud_rate;
    int data_bits;
    int stop_bits;
    char parity;
} uart_config_t;

static volatile int running = 1;
static int usb_fd = -1;
static int ama_fd = -1;

void signal_handler(int sig) {
    switch(sig) {
        case SIGTERM:
        case SIGINT:
            syslog(LOG_INFO, "Received signal %d, shutting down", sig);
            running = 0;
            break;
        case SIGHUP:
            syslog(LOG_INFO, "Received SIGHUP, ignoring");
            break;
    }
}

int configure_uart(int fd, uart_config_t *config) {
    struct termios tty;
    
    if (tcgetattr(fd, &tty) != 0) {
        syslog(LOG_ERR, "Error getting terminal attributes: %s", strerror(errno));
        return -1;
    }
    
    // Set baud rate
    speed_t speed;
    switch(config->baud_rate) {
        case 9600: speed = B9600; break;
        case 19200: speed = B19200; break;
        case 38400: speed = B38400; break;
        case 57600: speed = B57600; break;
        case 115200: speed = B115200; break;
        case 230400: speed = B230400; break;
        case 460800: speed = B460800; break;
        case 921600: speed = B921600; break;
        default:
            syslog(LOG_ERR, "Unsupported baud rate: %d", config->baud_rate);
            return -1;
    }
    
    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);
    
    // Configure data bits
    tty.c_cflag &= ~CSIZE;
    switch(config->data_bits) {
        case 5: tty.c_cflag |= CS5; break;
        case 6: tty.c_cflag |= CS6; break;
        case 7: tty.c_cflag |= CS7; break;
        case 8: tty.c_cflag |= CS8; break;
        default:
            syslog(LOG_ERR, "Unsupported data bits: %d", config->data_bits);
            return -1;
    }
    
    // Configure stop bits
    if (config->stop_bits == 2) {
        tty.c_cflag |= CSTOPB;
    } else {
        tty.c_cflag &= ~CSTOPB;
    }
    
    // Configure parity
    switch(config->parity) {
        case 'N':
        case 'n':
            tty.c_cflag &= ~PARENB;
            break;
        case 'E':
        case 'e':
            tty.c_cflag |= PARENB;
            tty.c_cflag &= ~PARODD;
            break;
        case 'O':
        case 'o':
            tty.c_cflag |= PARENB;
            tty.c_cflag |= PARODD;
            break;
        default:
            syslog(LOG_ERR, "Unsupported parity: %c", config->parity);
            return -1;
    }
    
    // Disable flow control
    tty.c_cflag &= ~CRTSCTS;
    
    // Enable receiver, ignore modem control lines
    tty.c_cflag |= CREAD | CLOCAL;
    
    // Disable canonical mode, echo, and signals
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    
    // Disable input processing
    tty.c_iflag &= ~(IXON | IXOFF | IXANY | ICRNL);
    
    // Raw output
    tty.c_oflag &= ~OPOST;
    
    // Set timeout (blocking read with timeout)
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 1; // 0.1 second timeout
    
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        syslog(LOG_ERR, "Error setting terminal attributes: %s", strerror(errno));
        return -1;
    }
    
    return 0;
}

int load_config(uart_config_t *config) {
    FILE *file = fopen(CONFIG_FILE, "r");
    
    // Set defaults
    strcpy(config->usb_uart_device, DEFAULT_USB_UART);
    strcpy(config->ama_uart_device, DEFAULT_AMA_UART);
    config->baud_rate = 115200;
    config->data_bits = 8;
    config->stop_bits = 1;
    config->parity = 'N';
    
    if (!file) {
        syslog(LOG_INFO, "Config file not found, using defaults");
        return 0;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n') continue;
        
        char key[128], value[128];
        if (sscanf(line, "%127[^=]=%127s", key, value) == 2) {
            // Remove whitespace
            char *k = key, *v = value;
            while (*k == ' ' || *k == '\t') k++;
            while (*v == ' ' || *v == '\t') v++;
            
            if (strcmp(k, "usb_uart_device") == 0) {
                strcpy(config->usb_uart_device, v);
            } else if (strcmp(k, "ama_uart_device") == 0) {
                strcpy(config->ama_uart_device, v);
            } else if (strcmp(k, "baud_rate") == 0) {
                config->baud_rate = atoi(v);
            } else if (strcmp(k, "data_bits") == 0) {
                config->data_bits = atoi(v);
            } else if (strcmp(k, "stop_bits") == 0) {
                config->stop_bits = atoi(v);
            } else if (strcmp(k, "parity") == 0) {
                config->parity = v[0];
            }
        }
    }
    
    fclose(file);
    return 0;
}

int create_pid_file(void) {
    FILE *pidfile = fopen(PID_FILE, "w");
    if (!pidfile) {
        syslog(LOG_ERR, "Cannot create PID file %s: %s", PID_FILE, strerror(errno));
        return -1;
    }
    fprintf(pidfile, "%d\n", getpid());
    fclose(pidfile);
    return 0;
}

void remove_pid_file(void) {
    unlink(PID_FILE);
}

void cleanup(void) {
    if (usb_fd >= 0) {
        close(usb_fd);
        usb_fd = -1;
    }
    if (ama_fd >= 0) {
        close(ama_fd);
        ama_fd = -1;
    }
    remove_pid_file();
    syslog(LOG_INFO, "Cleanup completed");
}

int daemonize(void) {
    pid_t pid;
    
    // Fork the first time
    pid = fork();
    if (pid < 0) {
        return -1;
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS); // Parent exits
    }
    
    // Create new session
    if (setsid() < 0) {
        return -1;
    }
    
    // Set signal handlers
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGHUP, signal_handler);
    signal(SIGCHLD, SIG_IGN);
    
    // Fork the second time
    pid = fork();
    if (pid < 0) {
        return -1;
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS); // Parent exits
    }
    
    // Set file permissions
    umask(0);
    
    // Change working directory
    if (chdir("/") < 0) {
        return -1;
    }
    
    // Close standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
    return 0;
}

int main(int argc, char *argv[]) {
    uart_config_t config;
    unsigned char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;
    int foreground = 0;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--foreground") == 0) {
            foreground = 1;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("Usage: %s [-f|--foreground] [-h|--help]\n", argv[0]);
            printf("  -f, --foreground  Run in foreground (don't daemonize)\n");
            printf("  -h, --help        Show this help message\n");
            return 0;
        }
    }
    
    // Open syslog
    openlog(DAEMON_NAME, LOG_PID, LOG_DAEMON);
    
    // Load configuration
    if (load_config(&config) < 0) {
        syslog(LOG_ERR, "Failed to load configuration");
        return EXIT_FAILURE;
    }
    
    syslog(LOG_INFO, "Starting UART bridge daemon");
    syslog(LOG_INFO, "USB UART: %s, AMA UART: %s, Baud: %d", 
           config.usb_uart_device, config.ama_uart_device, config.baud_rate);
    
    // Daemonize unless running in foreground
    if (!foreground) {
        if (daemonize() < 0) {
            syslog(LOG_ERR, "Failed to daemonize");
            return EXIT_FAILURE;
        }
    } else {
        // Set signal handlers for foreground mode
        signal(SIGTERM, signal_handler);
        signal(SIGINT, signal_handler);
        signal(SIGHUP, signal_handler);
    }
    
    // Create PID file
    if (create_pid_file() < 0) {
        syslog(LOG_ERR, "Failed to create PID file");
        return EXIT_FAILURE;
    }
    
    // Open USB UART
    usb_fd = open(config.usb_uart_device, O_RDWR | O_NOCTTY | O_SYNC);
    if (usb_fd < 0) {
        syslog(LOG_ERR, "Cannot open USB UART %s: %s", 
               config.usb_uart_device, strerror(errno));
        cleanup();
        return EXIT_FAILURE;
    }
    
    // Open AMA UART
    ama_fd = open(config.ama_uart_device, O_RDWR | O_NOCTTY | O_SYNC);
    if (ama_fd < 0) {
        syslog(LOG_ERR, "Cannot open AMA UART %s: %s", 
               config.ama_uart_device, strerror(errno));
        cleanup();
        return EXIT_FAILURE;
    }
    
    // Configure both UARTs
    if (configure_uart(usb_fd, &config) < 0) {
        syslog(LOG_ERR, "Failed to configure USB UART");
        cleanup();
        return EXIT_FAILURE;
    }
    
    if (configure_uart(ama_fd, &config) < 0) {
        syslog(LOG_ERR, "Failed to configure AMA UART");
        cleanup();
        return EXIT_FAILURE;
    }
    
    syslog(LOG_INFO, "UART bridge daemon started successfully");
    
    // Main loop
    while (running) {
        // Read from USB UART
        bytes_read = read(usb_fd, buffer, BUFFER_SIZE);
        if (bytes_read > 0) {
            // Write to AMA UART
            bytes_written = write(ama_fd, buffer, bytes_read);
            if (bytes_written != bytes_read) {
                syslog(LOG_WARNING, "Partial write to AMA UART: %zd/%zd bytes", 
                       bytes_written, bytes_read);
            }
        } else if (bytes_read < 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
            syslog(LOG_ERR, "Error reading from USB UART: %s", strerror(errno));
            break;
        }
        
        // Small delay to prevent excessive CPU usage
        usleep(1000); // 1ms
    }
    
    syslog(LOG_INFO, "UART bridge daemon shutting down");
    cleanup();
    closelog();
    
    return EXIT_SUCCESS;
}