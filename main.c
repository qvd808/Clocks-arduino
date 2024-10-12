#include <avr/io.h>
#include <util/delay.h>


void setup() {
    DDRB |= 0b00011111; // Set PB0-PB4 as output (EN, D4-D7)
    DDRD |= (1 << DDD7); // Set PD7 as output (RS)
}

void enable_pulse() {
    PORTB |= (1 << PORTB0);  // EN = 1
    _delay_us(1);             // Allow LCD to process
    PORTB &= ~(1 << PORTB0);  // EN = 0
    _delay_us(50);             // Allow LCD to process
}

void lcd_command(uint8_t cmd) {
    PORTD &= ~(1 << PORTD7); // RS = 0 for command mode

    PORTB = (PORTB & 0xE1) | (((cmd >> 4) & 0x0F) << 1);  // High nibble
    enable_pulse();
    PORTB = (PORTB & 0xE1) | ((cmd & 0x0F) << 1);  // Low nibble
    enable_pulse();
}

void lcd_data(uint8_t data) {
    PORTD |= (1 << PORTD7);  // RS = 1 for data mode

    PORTB = (PORTB & 0xE1) | (((data >> 4) & 0x0F) << 1);  // High nibble
    enable_pulse();
    PORTB = (PORTB & 0xE1) | ((data & 0x0F) << 1);  // Low nibble
    enable_pulse();
}

void clear_screen() {
    lcd_command(0x01); // Clear display
    _delay_ms(2);      // Increased delay for clear command
}

void initialize_lcd() {
    _delay_ms(50);   // Increased delay for power-up
    lcd_command(0x33); // Initialize in 8-bit mode first
    _delay_ms(5);    // Added delay
    lcd_command(0x32); // Switch to 4-bit mode
    _delay_ms(1);    // Added delay
    lcd_command(0x28); // 2 lines, 5x8 matrix
    _delay_ms(1);    // Added delay
    lcd_command(0x0C); // Display ON, cursor OFF
    _delay_ms(1);    // Added delay
    lcd_command(0x06); // Entry mode: cursor moves right
    _delay_ms(1);    // Added delay
    clear_screen();  // Clear display as part of initialization
}

void test() {
    clear_screen();   // Clear the display first
    _delay_ms(200);   // Short delay for the clear command
    lcd_data('B');    // Send 'B' to the LCD
    _delay_ms(1000);  // Wait for 1 second
    clear_screen();   // Clear the display again
    _delay_ms(1000);  // Wait for 1 second
}

int main(void) {
    setup();           // Initialize ports
    initialize_lcd();  // Set up the LCD
	
    while (1) {
        test();  // Run the test function continuously
    }
}
