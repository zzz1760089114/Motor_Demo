

/**
 * main.c
 */

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "Relay.h"
#include "Led.h"
#include "Key.h"
#include "Timer.h"
#include "ADC.h"
#include "LCD9648.h"
#include "SPI.h"
#include "ePWM.h"
#include "SCI.h"
#include "IIC.h"
//#include "Exti.h"

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
	KEY_Matrix_Init();
	ADC_Init();
	LCD9648_Init();
	SPIA_FIFO_Init();
	SCIA_FIFO_Init(4800);
	ePWM6_Init(500);
	Timer0_Init(150, 100000);
	Timer1_Init(150, 100000);
	IIC_Init();

	while (AT24CXX_Check())
	{
		SCI_FIFO_SendString("ATC4C02 Detection is Abnormal!\r\n");
		DELAY_US(1000);
		Clear_FIFO();
		DELAY_US(500*1000);
	}
	SCI_FIFO_SendString("ATC4C02 Detection is Normal!\r\n");
	DELAY_US(1000);
	Clear_FIFO();

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
		KEY_Matrix_ScanTask();
		KEY_ProcessEvent();
		DELAY_US(10*1000);
	}
}
