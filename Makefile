default:
	avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -I /usr/lib/avr/include -c -o main.o main.c
	avr-gcc -o led.bin main.o
	avr-objcopy -O ihex -R .eeprom led.bin led.hex
	sudo avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:led.hex
