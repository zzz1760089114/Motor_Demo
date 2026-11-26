#ifndef EXTI_H
#define EXTI_H

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File


void EXTI1_Init(void);
interrupt void EXTI1_IRQn(void);

#endif
