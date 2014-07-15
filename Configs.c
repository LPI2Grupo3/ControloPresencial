*
 * Configs.c
 *
 * Created: 12/07/2014 22:25:09
 *  Author: LPI2Grupo3 2013/2014
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define FOSC 16000000
#define BAUD 38400
#define MYUBR FOSC/16/BAUD-1

void SerieSetup(){
	
	UCSR0B |= (1<<TXEN0); // Enable Serial Transmission
	UCSR0C |= (1<<UCSZ01) | (1<<UCSZ00); // Asynchronous Mode
	UBRR0H = ((MYUBR)>>8);
	UBRR0L = MYUBR;
}

void putbuf (char c){
	UDR0 = c;
}

void TimerSetup(){
	
	TCCR1B |= (1<<WGM12);	// Timer 1 in CTC mode
	OCR1A = 280;			// 28.5 kHz * 2 = 57 kHz
	//OCR1A = 210;			// 38 kHz * 2 = 76 kHz
	TIMSK1 |= (1<<OCIE1A);	// Enable Output Compare Interrupt
}

void ADCSetup(){
	
	ADMUX |= (1<<REFS0);			// Select Vref=AVcc
	ADCSRA |= (1<<ADEN)|(1<<ADPS2);	// Set prescaler to /16 and enable ADC
}

unsigned int Analog_Read(unsigned char chan){
	
	ADMUX = (ADMUX & 0xF0)|(chan & 0x0F);	// Select ADC channel with safety mask
	ADCSRA |= (1<<ADSC);					// Single conversion mode
	while (ADCSRA & (1<<ADSC));				// Wait until ADC conversion is complete
	return ADC;
}

ISR(TIMER1_COMPA_vect){		// Timer 1 Compare Output Interrupt - IR Emitter Wave Generation
	
	static unsigned char pulses = 0, toff = 0;
	
	if(!toff){	// T_on: toggle IR emitter pins
		PORTD ^= (1<<PD5)|(1<<PD6);
		pulses++;
		if(pulses > 10){	// Limit number of pulses per cycle
			PORTD &= ~((1<<PD5)|(1<<PD6));	// Wave stays low
			pulses = 0;						// Reset pulse counter
			toff = 1;						// T_off flag activated
		}
	}
	else{		// T_off: wave stays low
		pulses++;
		if(pulses > 5){	// Limit number of dead zone (using "pulse" time units)
			PORTD |= (1<<PD5)|(1<<PD6);		// Wave goes high
			pulses = 0;						// Reset dead zone pulse counter
			toff = 0;						// T_off flag deactivated
		}
	}
}
