#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

int adc_read (void);
int main()
{
	//// ADC section

	DDRB &= ~(1<<PB5);  // wejscie ADC

	ADMUX = (1<<ADLAR) | (1<<MUX0);
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);

	//ADCSRA |= (1<<ADSC);

	//////////////////	wyjscia stanu baterii
	/*
	 * PB1=1 & PB0=1	->  Stan baterii wysoki
	 * PB1=0 & PB0=1	->	Stan baterii sredni
	 * PB1=0 & PB0=0	->  Stan baterii niski
	 */
	DDRB |= (1<<PB0);
	PORTB &= ~(1<<PB0);

	DDRB |= (1<<PB1);
	PORTB &= ~(1<<PB1);
	/////////////////////////////////////////////

	int adc_result = 0;


	while(1)
	{
		adc_result = adc_read();

		_delay_ms(50);

		/*
		 * Zmienic przedzialy
		 *
		 * > 12V   adc_result >= 105
		 * < 12 && > 11,4	adc_result < 105 && adc_result >= 95
		 * < 11,4	adc_result < 95
		 */

		if(adc_result > 103 )
		{
			PORTB |= (1<<PB0);
			PORTB |= (1<<PB1);
		}

		if(adc_result <= 103 && adc_result > 99)
		{
			PORTB |= (1<<PB0);
			PORTB &= ~(1<<PB1);
		}
		if(adc_result <= 99)
		{
			PORTB &= ~(1<<PB0);
			PORTB &= ~(1<<PB1);
		}

	}
}

int adc_read (void)
{
    int adc_result;



   // while(ADCSRA & (1<<ADSC));

    adc_result = ADCH;
     //adc_result = ADC; // For 10-bit resolution

    // Start the next conversion
    ADCSRA |= (1 << ADSC);

    return adc_result;
}
