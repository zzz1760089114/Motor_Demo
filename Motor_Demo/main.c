

/**
 * main.c
 */

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "Relay.h"
#include "Exti.h"
#include "Led.h"
#include "Timer.h"
#include "ADC.h"
#include "LCD9648.h"
#include "SPI.h"
#include "ePWM.h"
#include "SCI.h"

void main(void)
{
	int i=0;
	unsigned char fx = 0;

	InitSysCtrl();

	InitPieCtrl();
	IER = 0x0000;
	IFR = 0x0000;
	InitPieVectTable();

	Relay_Init();
	Led_Init();
	ADC_Init();
	LCD9648_Init();
	SPIA_FIFO_Init();
	SCIA_FIFO_Init(4800);
	ePWM6_Init(500);
	Timer0_Init(150, 100000);
	Timer1_Init(150, 100000);
	EXTI1_Init();

	while(1)
	{
		if(fx==0)
		{
			i++;
			if(i==300)
			{
				fx=1;
			}
		}
		else
		{
			i--;
			if(i==0)
			{
				fx=0;
			}
		}
		ePwm6A_SetCompare(i); //i值最大可以取499，因为ARR最大值是499.
		ePwm6B_SetCompare(300-i); //i值最大可以取499，因为ARR最大值是499.
		KEY1_DebounceTask();
		DELAY_US(10*1000);
	}
}
