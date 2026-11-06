#ifndef ADC_H
#define ADC_H

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

#define ADC_MODCLK (3)
#define ADC_CKPS (0x01)
#define ADC_SHCLK (0x0f)

void ADC_Init(void);
Uint16 Read_ADC_Value(void);

#endif
