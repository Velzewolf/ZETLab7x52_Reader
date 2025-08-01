# Makefile for ZET Lab Serial Communication Tool

CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LIBS = 
TARGET = zetlab_reader
SOURCE = zetlab_reader.c

# Windows-specific settings
ifeq ($(OS),Windows_NT)
    TARGET := $(TARGET).exe
    LIBS += -lkernel32
else
    # For cross-compilation on Linux for Windows
    CC = x86_64-w64-mingw32-gcc
    TARGET := $(TARGET).exe
    LIBS += -lkernel32
endif

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE) $(LIBS)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

help:
	@echo "Available targets:"
	@echo "  all      - Build the executable"
	@echo "  clean    - Remove built files"
	@echo "  install  - Install to /usr/local/bin"
	@echo "  help     - Show this help message"