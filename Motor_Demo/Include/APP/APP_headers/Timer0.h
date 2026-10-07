#ifndef TIMER0_H
#define TIMER0_H

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

void Timer0_Init(float Freq, float Period);
interrupt void Timer0_TRQn(void);

#endif
