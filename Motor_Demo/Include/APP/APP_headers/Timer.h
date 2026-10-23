#ifndef TIMER_H
#define TIMER_H

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

void Timer0_Init(float Freq, float Period);
void Timer1_Init(float Freq, float Period);
interrupt void Timer0_TRQn(void);
interrupt void Timer1_TRQn(void);

#endif
