/*
 * LPI2_Slave.c
 *
 * Created: 12/07/2014 22:23:50
 *  Author: TiagoSousa
 */ 

#include <avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>
#include <avr/interrupt.h>
#include "Configs.h"

volatile unsigned int ADC_in = 0, ADC_out = 0;

int main(){
	
	unsigned char InSensorInterrupted = 0, OutSensorInterrupted = 0;
	unsigned char EnterSequence = 0, ExitSequence = 0;
	
	DDRD |= (1<<PD5)|(1<<PD6);		// IR Emitter Waveform Generation Pins
	
	SerieSetup();
	ADCSetup();
	TimerSetup();
	sei();
	TCCR1B |= (1<<CS10);	// Start Waveform Generation
	
	while(1){
		
		ADC_out = Analog_Read(0);		// Read outer sensor (IR Receiver connected to pin PC0)
		if (ADC_out < 600) OutSensorInterrupted = 0;		// If the ADC value is low, sensor is not interrupted
		else OutSensorInterrupted = 1;
		
		ADC_in = Analog_Read(1);		// Read inner sensor (IR Receiver connected to pin PC1)
		if (ADC_in < 600) InSensorInterrupted = 0;		// If the ADC value is low, sensor is not interrupted
		else InSensorInterrupted = 1;
		
		// Sequence Detection Algorithm
		if (OutSensorInterrupted && !InSensorInterrupted){
			if (!ExitSequence && (!EnterSequence || EnterSequence == 2))
			EnterSequence = 1;
			if (ExitSequence == 2)
			ExitSequence++;
		}
		else if (OutSensorInterrupted && InSensorInterrupted){
			if (EnterSequence == 1 || EnterSequence == 3)
			EnterSequence = 2;
			if (ExitSequence == 1 || ExitSequence == 3)
			ExitSequence = 2;
		}
		else if (!OutSensorInterrupted && InSensorInterrupted){
			if (!EnterSequence && (!ExitSequence || ExitSequence == 2))
			ExitSequence = 1;
			if (EnterSequence == 2)
			EnterSequence++;
		}
		else if (!OutSensorInterrupted && !InSensorInterrupted){
			if (EnterSequence == 3) putbuf('E');
			if (ExitSequence == 3) putbuf('S');
			EnterSequence = 0;
			ExitSequence = 0;
		}
	}
	
	return 0;
}
