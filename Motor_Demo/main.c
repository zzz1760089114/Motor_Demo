

/**
 * main.c
 */

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "Relay.h"
#include "Exti.h"
#include "Led.h"
#include "Timer0.h"
#include "ADC.h"

void main()
{
	//int i=0;

	float ADC_Vol;

	InitSysCtrl();

	InitPieCtrl();
	IER = 0x0000;
	IFR = 0x0000;
	InitPieVectTable();

	Relay_Init();
    EXTI1_Init();
	Led_Init();
	Timer0_Init(150, 500000);
	ADC_Init();

	while(1)
	{
		ADC_Vol = (float)Read_ADC_Value()*3.0/4095;
		//SMG_DisplayFloat(adc_vol,2);
		//SMG_DisplayInt(Read_ADCValue());
	}
}
