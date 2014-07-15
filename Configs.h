/*
 * Configs.h
 *
 * Created: 12/07/2014 22:24:25
 *  Author: LPI2Grupo3 2013/2014
 */ 

#ifndef CONFIGS_H_
#define CONFIGS_H_

#define FOSC 16000000
#define BAUD 38400
#define MYUBR FOSC/16/BAUD-1

void SerieSetup();
void putbuf(char c);
void TimerSetup();
void ADCSetup();
unsigned int Analog_Read(unsigned char chan);

#endif /* CONFIGS_H_ */
