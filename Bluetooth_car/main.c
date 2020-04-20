/*
 * Dodac macierz [100, 100]
 * Ruch obliczac na zasadzie opisu punktow na osiach ukladu wspolrzednych XY
 * ruch do przodu +/- os Y
 * skrecanie +/- os X
 * Powrot do miejsca poprzez odwrocenie akcji (jazda do przodu -> jazda do tylu)
 *
 *
 *
 */
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#define UART_BAUD 38400//9600
#define __UBRR ((F_CPU+UART_BAUD*8UL) / (16UL*UART_BAUD)-1)

void USART_Init( uint16_t ubrr);
void Usart_Transmit(unsigned char data);
void Send_clause(char * napis);
unsigned char USART_Receive( void );
void komunikacja(void);
void Send(char dane[50]);
void BT_Init();
void PWM_Init();
void ServoPWM_Init();

char dane[7];
int l = 0, ile=0, turbo = 0;
char poprzedni = '5';
char thr_brk = '0';

volatile int odleglosc = 0;
char jazda = '1';

volatile int tor[60], iterator = 0, dekrementator = 58;
volatile int Y=0, Poprzedni_Y = 0;

volatile uint8_t Battery_status = 3;

int main(void)
{
	l=0;
	USART_Init(__UBRR);
	_delay_ms(100);
	PWM_Init();
	ServoPWM_Init();
	//BT_Init();

	DDRB |= (1<<PB1); 	// Pin B1 -> AIN1 sterownik
	PORTB |= (1<<PB1);

	DDRB |= (1<<PB0);	// Pin B0 -> AIN2 sterownik
	PORTB |= (1<<PB0);

	DDRB |= (1<<PB6);	// Pin B6 -> dodatkowy led stopu
	PORTB |= (1<<PB6);

	DDRD |= (1<<PD2);	// Pin D2 -> ledy cofania
	PORTD &= ~(1<<PD2);

	DDRA |= (1<<PA0);	// Pin A0 -> Buzzer
	PORTA &= ~(1<<PA0);


	//////// sekcja pomiaru stanu baterii

	DDRD |= (1<<PD3); // Sygnal wyjsciowy rozpoczynajacy pomiar
	PORTD &= ~(1<<PD3);

	DDRD &= ~(1<<PD4);	// wejscie 1
	PORTD &= ~(1<<PD4);

	DDRD &= ~(1<<PD5);	// wejscie 2
	PORTD &= ~(1<<PD5);

	///////////

	PORTA |=(1<<PA0);
	_delay_ms(200);
	PORTA &= ~(1<<PA0);


	while(1)
	{
		komunikacja();

		/*if((PIND & (1<<PD2)))
			jazda = '0';
		else
			jazda = '1'; */
	} // end while
} // end main

void PWM_Init()
{
	    DDRB   |= (1 << PB2);                   // Pin B2 -> PWM silnik

	    TCCR0A = (1 << COM0A1) | (1 << WGM00) | (1 << WGM01);  // WGM01 Fast PWM //phase correct PWM mode

	    TCCR0B = (1 << CS01);   // clock source = CLK/8, start PWM
}

void ServoPWM_Init()
{
	//  OC1B outputs
			DDRB |= (1<<PB4);
			// TOP, set for 50Hz (20ms)
			ICR1 = 19999;
			// Center outputs (1.5ms)
			OCR1B = 1550;//1000;//1500;
			// Timer 1 fast PWM mode 14
			// Clear on compare, set at TOP
			// /8 prescaler
			TCCR1A = (1<<COM1A1)|(1<<COM1B1)|(1<<WGM11);
			TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS11);


			_delay_ms(1500);
}


void USART_Init( unsigned int ubrr)
{
		UBRRH = (unsigned char)(ubrr>>8);
		UBRRL = (unsigned char)ubrr;
		UCSRB = (1<<TXEN) | (1<<RXEN);
		UCSRC = (3<<UCSZ0);
}

void Usart_Transmit(unsigned char data)
{
	while ( !( UCSRA & (1<<UDRE)) );
	UDR = data;
	}

unsigned char USART_Receive( void )
{
/* Wait for data to be received */
while ( !(UCSRA & (1<<RXC)) )
;
/* Get and return received data from buffer */
return UDR;
}

void Send_clause(char * napis)
{
	while(*napis)
		Usart_Transmit(*napis++);
}

void Send(char dane[50])
{
	int length = strlen(dane);

	for(int i=0; i<length; i++)
		Usart_Transmit(dane[i]);
	}


void BT_Init()
{
	Send_clause("AT+RESET");
	_delay_ms(150);
	Send_clause("AT+START");
	_delay_ms(150);
	}


void komunikacja(void)
{
	unsigned char odbior = USART_Receive();

	dane[l]= odbior;
	l++;

	if(l==3)
	{
		// dane[0] - Przod tyl
		// dane[1] - Prawo lewo
		// dane[2] - klakson


		if(dane[0] == 'H' && dane[1] == 'I' && dane[2] == '!')
		{
			PORTA |=(1<<PA0);
			_delay_ms(200);
			PORTA &= ~(1<<PA0);
			_delay_ms(500);
			PORTA |=(1<<PA0);
			_delay_ms(200);
			PORTA &= ~(1<<PA0);
			_delay_ms(300);
		}

		// Hamowanie
		if(dane[0] == '5' && thr_brk != '5')
		{
			for(int i = 200; i > 10; i--)
			{
				OCR0A  = i;
				_delay_ms(3);
			}


			////////////////////
			char bs[2];
			bs[0] = 'B';
			bs[1] = 'S';
			bs[2] = 'X';

			if((PIND & (1<<PD4)) && (PIND & (1<<PD5)))
			{
				Battery_status = 3;
				bs[2] = 'C';
			}

			if((PIND & (1<<PD4)) && !(PIND & (1<<PD5)))
			{
				Battery_status = 2;
				bs[2] = 'B';
			}

			if(!(PIND & (1<<PD4)) && !(PIND & (1<<PD5)))
			{
				Battery_status = 1;
				bs[2] = 'A';
			}

			Send(bs);
			_delay_ms(10);

			////////////////////////
		}

		// Rozpedzanie
		/*if((dane[0] == '1' || dane[0] == '2') && thr_brk == '0')
		{
			if(dane[0] == '1')
				PORTB &= ~(1<<PB1); // lewo

			if(dane[0] == '2')
				PORTB |= (1<<PB1);// prawo

			PORTB |= (1<<PB0);

			for(int i = 50; i < 255; i++)
			{
					OCR0A  = i;
					_delay_ms(2);
			}
		} */



	/*	if(dane[0] == '0' && thr_brk == '0')
		{
			turbo = 0;

			PORTB |= (1<<PB1); // soft stop

			PORTB &= ~(1<<PB0);

			OCR0A  = 0;

			PORTB |= (1<<PB6); // dodatkowy led stopu wlaczone

			PORTD &= ~(1<<PD4);
		}
		else
		{
			PORTB &= ~(1<<PB6); // dodatkowy led stopu wylaczone
		}

		if(dane[0]=='1' && dane[2]=='0' && thr_brk != '0')
		{
			PORTB &= ~(1<<PB1); // lewo

			PORTB |= (1<<PB0);

			if(turbo < 5)
				OCR0A  = 255;
			else
				OCR0A  = 170;

			PORTD &= ~(1<<PD4);
		}

		if(dane[0]=='2' && dane[2]=='0' && thr_brk != '0')
		{
			PORTB |= (1<<PB1);// prawo

			PORTB |= (1<<PB0);

			if(turbo < 5)
				OCR0A  = 255;
			else
				OCR0A  = 170;

			PORTD |= (1<<PD4);
		}

		if(dane[0]=='1' && dane[2]=='1' && thr_brk != '0')
		{
			PORTB &= ~(1<<PB1); // lewo

			PORTB |= (1<<PB0);

			OCR0A  = 255;

			PORTD &= ~(1<<PD4);
		}

		if(dane[0]=='2' && dane[2]=='1' && thr_brk != '0')
		{
			PORTB |= (1<<PB1);// prawo

			PORTB |= (1<<PB0);

			OCR0A  = 255;

			PORTD |= (1<<PD4);
		} */


		// Klakson
		if(dane[2] == '1')
		{
			//PORTA |=(1<<PA0);

			PORTA |= (1<<PA0);
		}
		else
		{
			PORTA &= ~(1<<PA0);
		}


		switch(dane[0])
		{
		case '9': {	PORTB &= ~(1<<PB1); // przod
					PORTB |= (1<<PB0);
						OCR0A  = 255;
					PORTD &= ~(1<<PD2);
					PORTB &= ~(1<<PB6);
					}; break;
		case '8': {	PORTB &= ~(1<<PB1); // przod
					PORTB |= (1<<PB0);
						OCR0A  = 210;
					PORTD &= ~(1<<PD2);
					PORTB &= ~(1<<PB6);
					}; break;
		case '7': {	PORTB &= ~(1<<PB1); // przod
					PORTB |= (1<<PB0);
						OCR0A  = 195;
					PORTD &= ~(1<<PD2);
					PORTB &= ~(1<<PB6);
					}; break;
		case '6': {	PORTB &= ~(1<<PB1); // przod
					PORTB |= (1<<PB0);

						OCR0A  = 170;
					PORTD &= ~(1<<PD2);
					PORTB &= ~(1<<PB6);
					}; break;


		case '5': {	OCR0A  = 0;
					PORTB &= ~(1<<PB1);
					PORTB &= ~(1<<PB0);
					PORTB |= (1<<PB6); // dodatkowy led stopu wlaczone
					PORTD &= ~(1<<PD2);
					turbo = 0;
					}; break;


		case '4': { PORTB |= (1<<PB1);// tyl
					PORTB &= ~(1<<PB0);
					OCR0A  = 170;
					Y--;
					PORTD |= (1<<PD2);
					PORTB &= ~(1<<PB6);
					}; break;
		case '3': {	PORTB |= (1<<PB1);// tyl
					PORTB &= ~(1<<PB0);
					OCR0A  = 195;
					PORTD |= (1<<PD2);
					PORTB &= ~(1<<PB6);
					}; break;
		case '2': {	PORTB |= (1<<PB1);// tyl
					PORTB &= ~(1<<PB0);
					OCR0A  = 215;
					PORTD |= (1<<PD2);
					PORTB &= ~(1<<PB6);
					}; break;
		case '1': {	PORTB |= (1<<PB1);// tyl
					PORTB &= ~(1<<PB0);
					OCR0A  = 255;

					PORTD |= (1<<PD2);
					PORTB &= ~(1<<PB6);
					}; break;
		}


			switch(dane[1])
			{
			case '5': OCR1B = 1450; break; // 1500

			case '4': OCR1B = 1450; break; // 1250
			case '3': OCR1B = 1150; break; // 1000
			case '2': OCR1B = 1150; break; // 750
			case '1': OCR1B = 750; break;  // 500

			case '6': OCR1B = 1450; break; // 1750
			case '7': OCR1B = 1850; break; // 2000
			case '8': OCR1B = 1850; break; // 2250
			case '9': OCR1B = 2420; break; // 2500
			}

			if(dane[1] == '5' && (uint8_t)poprzedni < (uint8_t) '5')
			{
				OCR1B = 1700;
			}

			if(dane[1] == '5' && (uint8_t)poprzedni > (uint8_t) '5')
			{
				OCR1B = 1350;
			}

			if(turbo < 7)
				turbo++;


			// coœ nie dziala
		/*iterator++;


		if(iterator >= 1000)
		{
			PORTA |= (1<<PA0);
			_delay_ms(1000);
			PORTA &= ~(1<<PA0);

			Y+=100;


			while(Y >= 1)
			{


				if(Y > 0)
				{
					OCR0A = 185;
					PORTB |= (1<<PB1);// tyl
				}
				else
				{
					OCR0A = 185;
					PORTB &= ~(1<<PB1);// przod
				}
				_delay_ms(20);
				Y--;
			}

			PORTA &= ~(1<<PA0);

		} */


		//Send_clause("BSA");


			//Usart_Transmit('B');

		Poprzedni_Y = Y;
		poprzedni = dane[1];
		thr_brk = dane[0];
		l=0;
	}
	}



