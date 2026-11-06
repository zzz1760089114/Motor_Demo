#ifndef EXTI_H
#define EXTI_H

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

#define KEY_H1			(GpioDataRegs.GPADAT.bit.GPIO12)

extern volatile Uint16 Key_Count;

void EXTI1_Init(void);
void KEY1_DebounceTask(void);
interrupt void EXTI1_IRQn(void);

#endif
