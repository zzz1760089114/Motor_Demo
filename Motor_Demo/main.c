

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
#include "SPI.h"

void main(void)
{
	//int i=0;
	Uint16 Tx_Data = 0;
	Uint16 Rx_Data = 0;
	Uint16 Tx_data[4];
	Uint16 Rx_data[4];

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
	SPIA_FIFO_Init();

	Tx_Data = 20;
	Tx_data[0] = 20;
	Tx_data[1] = 30;
	Tx_data[2] = 40;
	Tx_data[3] = 50;
	//Rx_Data = SPI_Transfer8(Tx_Data);
	SPI_FIFO_Transfer8(&Tx_Data, &Rx_Data, 1);
	SPI_FIFO_Transfer8(Tx_data, Rx_data, 4);

	while(1)
	{
		ADC_Vol = (float)Read_ADC_Value()*3.0/4095;
		LCD9648_ShowString(0U, 0U, "Motor Demo");
		LCD9648_ShowString(0U, 1U, "Voltage:");
		LCD9648_ShowDecimal(48U, 1U, ADC_Vol, 1U, 2U);
		LCD9648_ShowNumber(0U, 2U, Rx_Data, 8U);
		LCD9648_ShowNumber(0U, 3U, Rx_data[0], 2U);
		LCD9648_ShowNumber(18U, 3U, Rx_data[1], 2U);
		LCD9648_ShowNumber(36U, 3U, Rx_data[2], 2U);
		LCD9648_ShowNumber(54U, 3U, Rx_data[3], 2U);
	}
}
