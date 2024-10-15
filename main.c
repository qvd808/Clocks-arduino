#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Time settings
volatile uint8_t hours = 11;
volatile uint8_t minutes = 34;
volatile uint8_t seconds = 0;

// Function to pulse the Enable pin of the LCD
void enable_pulse() {
    PORTB |= (1 << PORTB0);  // Set EN = 1
    _delay_us(1);             // Short delay
    PORTB &= ~(1 << PORTB0);  // Set EN = 0
    _delay_ms(2);             // Allow command to execute
}

// Send a command to the LCD
void lcd_command(uint8_t cmd) {
    PORTD &= ~(1 << PORTD7);  // RS = 0 (command mode)

    // Send high nibble
    PORTB = (PORTB & 0xE1) | ((cmd >> 4) << 1);
    enable_pulse();

    // Send low nibble
    PORTB = (PORTB & 0xE1) | ((cmd & 0x0F) << 1);
    enable_pulse();
}

// Send data to the LCD
void lcd_data(uint8_t data) {
    PORTD |= (1 << PORTD7);  // RS = 1 (data mode)

    // Send high nibble
    PORTB = (PORTB & 0xE1) | ((data >> 4) << 1);
    enable_pulse();

    // Send low nibble
    PORTB = (PORTB & 0xE1) | ((data & 0x0F) << 1);
    enable_pulse();
}

// Clear the LCD screen
void clear_screen() {
    lcd_command(0x01);  // Clear display command
    _delay_ms(2);       // Delay to complete the operation
}

// Initialize the LCD
void initialize_lcd() {
    _delay_ms(50);      // Allow LCD time to power up
    lcd_command(0x33);  // Initialize to 8-bit mode
    _delay_ms(5);
    lcd_command(0x32);  // Switch to 4-bit mode
    _delay_ms(1);
    lcd_command(0x28);  // 2 lines, 5x8 matrix
    lcd_command(0x0C);  // Display ON, cursor OFF
    lcd_command(0x06);  // Entry mode set: increment cursor
    clear_screen();     // Clear display
}

// Display time on the LCD
void display_time(uint8_t h, uint8_t m, uint8_t s) {
    lcd_command(0x80);  // Set cursor to the first line

    // Display hours
    lcd_data('0' + h / 10);
    lcd_data('0' + h % 10);
    lcd_data(':');

    // Display minutes
    lcd_data('0' + m / 10);
    lcd_data('0' + m % 10);
    lcd_data(':');

    // Display seconds
    lcd_data('0' + s / 10);
    lcd_data('0' + s % 10);
}

void setup_timer() {
    TCCR1A = 0;  // Normal mode
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);  // CTC mode, prescaler 1024
    OCR1A = 15624;  // For 1 second interrupt at 16 MHz
    TIMSK1 |= (1 << OCIE1A);  // Enable compare match interrupt
}

// Timer compare match interrupt handler
ISR(TIMER1_COMPA_vect) {
    seconds++;
    if (seconds >= 60) {
        seconds = 0;
        minutes++;
        if (minutes >= 60) {
            minutes = 0;
            hours = (hours + 1) % 24;
        }
    }
}

// Main function
int main(void) {
    DDRB |= 0b00011111;  // Set PB0-PB4 as output
    DDRD |= (1 << DDD7);  // Set PD7 as output

    initialize_lcd();  // Initialize LCD
    setup_timer();     // Setup timer
    sei();             // Enable global interrupts

    while (1) {
        // Disable interrupts to safely access shared variables
        cli();
        uint8_t current_hours = hours;
        uint8_t current_minutes = minutes;
        uint8_t current_seconds = seconds;
        sei();  // Re-enable interrupts

        display_time(current_hours, current_minutes, current_seconds);
        _delay_ms(100);  // Small delay to prevent too frequent updates
    }
}

