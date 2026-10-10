

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
#include "LCD9648.h"

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
    //EXTI1_Init();
	Led_Init();
	Timer0_Init(150, 100000);
	ADC_Init();
	LCD9648_Init();
	//LCD9648_Fill(0xAAU);   /* Power-on test: alternating horizontal lines. */

	while(1)
	{
		ADC_Vol = (float)Read_ADC_Value()*3.0/4095;
		LCD9648_ShowString(0U, 0U, "Motor Demo");
		LCD9648_ShowString(0U, 2U, "Voltage:");
		LCD9648_ShowDecimal(48U, 2U, ADC_Vol, 1U, 2U);
		//SMG_DisplayFloat(adc_vol,2);
		//SMG_DisplayInt(Read_ADCValue());
	}
}
