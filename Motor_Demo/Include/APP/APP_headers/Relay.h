#ifndef RELAY_H
#define RELAY_H

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

#define Relay_On (GpioDataRegs.GPASET.bit.GPIO15 = 1)
#define Relay_Off (GpioDataRegs.GPACLEAR.bit.GPIO15 = 1)
#define Relay_Toggle (GpioDataRegs.GPATOGGLE.bit.GPIO15 = 1)

void Relay_Init(void);

#endif
