#ifndef EPWM_H
#define EPWM_H

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

void ePWM6_Init(Uint16 tbprd);
void ePwm6A_SetCompare(Uint16 val);
void ePwm6B_SetCompare(Uint16 val);

#endif
