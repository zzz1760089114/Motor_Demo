

/**
 * main.c
 */

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "Relay.h"
#include "Exti.h"
#include "Led.h"
#include "Timer0.h"

void main()
{
	int i=0;

	InitSysCtrl();

	InitPieCtrl();
	IER = 0x0000;
	IFR = 0x0000;
	InitPieVectTable();

	Relay_Init();
//  EXTI1_Init();
	Led_Init();
	Timer0_Init(150, 500000);

	while(1)
	{
		i++;
		if(i%2000==0)
		{
			Led1_Toggle;
		}
		DELAY_US(100);
	}
}
