#ifndef LED_H
#define LED_H

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

#define Led1_Toggle (GpioDataRegs.GPCTOGGLE.bit.GPIO68 = 1)
#define Led2_Toggle (GpioDataRegs.GPCTOGGLE.bit.GPIO67 = 1)

void Led_Init(void);

#endif
