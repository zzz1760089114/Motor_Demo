//###########################################################################
//
// FILE:	DSP2833x_Sci.c
//
// TITLE:	DSP2833x SCI Initialization & Support Functions.
//
//###########################################################################
// $TI Release: F2833x/F2823x Header Files and Peripheral Examples V142 $
// $Release Date: November  1, 2016 $
// $Copyright: Copyright (C) 2007-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

//
// Included Files
//
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

//
// InitSci - This function initializes the SCI(s) to a known state.
//
void InitSci(void)
{
    //
    // Initialize SCI-B
    //
}	

//
// InitSciGpio - This function initializes GPIO to function as SCI-A, SCI-B, or
// SCI-C
//
// Each GPIO pin can be configured as a GPIO pin or up to 3 different
// peripheral functional pins. By default all pins come up as GPIO
// inputs after reset.  
// 
// Caution: 
// Only one GPIO pin should be enabled for SCITXDA/B operation.
// Only one GPIO pin shoudl be enabled for SCIRXDA/B operation. 
// Comment out other unwanted lines.
//
void InitSciGpio()
{
    InitScibGpio();
}

void InitScibGpio()
{
    EALLOW;

    //
    // Enable internal pull-up for the selected pins 
    // Pull-ups can be enabled or disabled disabled by the user.  
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.

    GpioCtrlRegs.GPAPUD.bit.GPIO18 = 0;	 //Enable pull-up for GPIO18 (SCITXDB)
    GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;	 //Enable pull-up for GPIO19 (SCIRXDB)

    GpioCtrlRegs.GPAQSEL2.bit.GPIO19 = 3;  // Asynch input GPIO19 (SCIRXDB)

    GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 2;  //Configure GPIO18 to SCITXDB
    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 2;   //Configure GPIO19 for SCIRXDB

    //RS485_EN∂Àø⁄≈‰÷√
    GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO61 = 1;
    GpioCtrlRegs.GPBPUD.bit.GPIO61 = 0;
    GpioDataRegs.GPBSET.bit.GPIO61 = 1;
    EDIS;
}

void scic_fifo_init()
{
  /*  ScicRegs.SCIFFTX.all=0xA040;
    ScicRegs.SCIFFRX.all=0x0028;
    ScicRegs.SCIFFCT.all=0x0;*/

    ScicRegs.SCIFFTX.all=0xE040;//0xE040;
    ScicRegs.SCIFFRX.all=0x2061;//0x204f;//
    ScicRegs.SCIFFCT.all=0x0;
}
// Test 1,SCIA  DLB, 8-bit word, baud rate 0x000F, default, 1 STOP bit, no parity
void scic_echoback_init()
{
    // Note: Clocks were turned on to the SCIA peripheral
    // in the InitSysCtrl() function
    ScicRegs.SCICCR.all =0x0007;   // 1 stop bit,  No loopback
                                   // No parity,8 char bits,
                                   // async mode, idle-line protocol
    ScicRegs.SCICTL1.all =0x0003;  // enable TX, RX, internal SCICLK,
                                   // Disable RX ERR, SLEEP, TXWAKE
    ScicRegs.SCICTL2.all =0x0003;
    ScicRegs.SCICTL2.bit.TXINTENA = 1;
    ScicRegs.SCICTL2.bit.RXBKINTENA =1;
    #if (CPU_FRQ_150MHZ)
          //ScicRegs.SCIHBAUD    =0x0001;  // 9600 baud @LSPCLK = 37.5MHz.
          //ScicRegs.SCILBAUD    =0x00E7;
          ScicRegs.SCIHBAUD    =0x0000;  // 115200 baud @LSPCLK = 15MHz.
          ScicRegs.SCILBAUD    =0x000F;
    #endif
    #if (CPU_FRQ_100MHZ)
      ScicRegs.SCIHBAUD    =0x0001;  // 9600 baud @LSPCLK = 20MHz.
      ScicRegs.SCILBAUD    =0x0044;
    #endif
    ScicRegs.SCICTL1.all =0x0023;  // Relinquish SCI from Reset
    ScicRegs.SCIFFRX.bit.RXFIFORESET=1;
}
/********************************************************************************
name:   void Send_Btye(unsigned char SendBtye)
input:  SendBtye
*********************************************************************************/
void Send_Btye(unsigned char SendBtye)
{
    while (ScicRegs.SCIFFTX.bit.TXFFST != 0) {}
    ScicRegs.SCITXBUF=SendBtye;
}

//void Send_String(char *msg)
