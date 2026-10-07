#include "Exti.h"
#include "Relay.h"
#include "Led.h"

void EXTI1_Init(void)
{
    EALLOW;
    SysCtrlRegs.PCLKCR3.bit.GPIOINENCLK = 1;
    EDIS;

    EALLOW;
    GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO12 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0;
    GpioCtrlRegs.GPAQSEL1.bit.GPIO12 = 0;
    EDIS;

    EALLOW;
    GpioCtrlRegs.GPBMUX2.bit.GPIO48 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO48 = 1;
    GpioCtrlRegs.GPBPUD.bit.GPIO48 = 0;
    GpioDataRegs.GPBCLEAR.bit.GPIO48 = 1;
    EDIS;
    
    EALLOW;
    GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = 12;
    EDIS;

    EALLOW;
    PieVectTable.XINT1 = &EXTI1_IRQn;
    EDIS;

    PieCtrlRegs.PIEIER1.bit.INTx4 = 1;
    
    XIntruptRegs.XINT1CR.bit.POLARITY = 0;
    XIntruptRegs.XINT1CR.bit.ENABLE = 1;
    
    IER |= M_INT1;
    EINT;
    ERTM;
}

interrupt void EXTI1_IRQn(void)
{
	Uint32 i;
	for(i=0;i<10000;i++);    //键盘消抖动
	while(!KEY_H1);
	Led1_Toggle;
	PieCtrlRegs.PIEACK.bit.ACK1=1;
}
