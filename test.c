#include <avr/io.h>
#include <util/delay.h>

// Assuming you're using a UART for debugging
#define BAUD 9600
#define F_CPU 16000000UL  // Adjust this to your AVR's clock speed
#include <util/setbaud.h>

void uart_init() {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;
    UCSR0B = (1<<TXEN0);
    UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
}

void uart_putchar(char c) {
    while (!(UCSR0A & (1<<UDRE0)));
    UDR0 = c;
}

void uart_puts(const char* s) {
    while (*s) {
        uart_putchar(*s++);
    }
}

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
    uart_puts("Clearing screen\r\n");
    clear_screen();
    _delay_ms(200);

    uart_puts("Printing 'A'\r\n");
    lcd_data('B');
    _delay_ms(1000);

    uart_puts("Clearing screen again\r\n");
    clear_screen();
    _delay_ms(200);

    uart_puts("Printing 'Hello'\r\n");
    lcd_data('H');
    lcd_data('e');
    lcd_data('l');
    lcd_data('l');
    lcd_data('o');
    _delay_ms(1000);
}

int main(void) {
    uart_init();
    uart_puts("Starting LCD test\r\n");

    setup();
    uart_puts("Ports initialized\r\n");

    initialize_lcd();
    uart_puts("LCD initialized\r\n");

    while (1) {
        test();
        _delay_ms(3000);  // Wait 3 seconds before repeating
    }
}
