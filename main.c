#include <avr/io.h>
#include <util/delay.h>

int main(void) {
	// set portb5 as an output
	DDRB = DDRB | (1 << DDB5);

	//main loop
	while (1) {
		// set portb5
		PORTB =  PORTB | (1 << PORTB5);
		//wait
		_delay_ms(1000);
		//unset portb5
		PORTB = PORTB & ~(1 << PORTB5);

		// wait somemore
		_delay_ms(1000);

	}

}
