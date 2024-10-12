# MCU name
MCU = atmega328p

# Programmer type
PROGRAMMER = arduino

# Serial port
SERIAL_PORT = /dev/ttyACM0

# Baud rate
BAUD_RATE = 115200

# Source files
SOURCES = main.c

# Compiler flags
CFLAGS = -mmcu=$(MCU) -DF_CPU=16000000UL -Os -I/usr/lib/avr/include

# Object files
OBJECTS = $(SOURCES:.c=.o)

# Executable name
TARGET = led

# Compiler
CC = avr-gcc

# Hex file creator
OBJCOPY = avr-objcopy

# Uploader
AVRDUDE = avrdude

all: $(TARGET).hex

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).bin: $(OBJECTS)
	$(CC) -mmcu=$(MCU) -o $@ $^

$(TARGET).hex: $(TARGET).bin
	$(OBJCOPY) -O ihex -R .eeprom $< $@

upload: $(TARGET).hex
	$(AVRDUDE) -F -V -c $(PROGRAMMER) -p $(MCU) -P $(SERIAL_PORT) -b $(BAUD_RATE) -U flash:w:$<

clean:
	rm -f $(OBJECTS) $(TARGET).bin $(TARGET).hex

.PHONY: all clean upload
