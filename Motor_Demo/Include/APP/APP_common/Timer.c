#include "Timer.h"
#include "Led.h"
#include "LCD9648.h"
#include "SPI.h"

void Timer0_Init(float Freq, float Period)
{
    EALLOW;
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER0ENCLK = 1;
    EDIS;

    CpuTimer0.RegsAddr = &CpuTimer0Regs;    //指向定时器0的寄存器地址，这句话是用来供通用函数使用，例如ConfigCpuTimer(&CpuTimer0, Freq, Period)

    CpuTimer0Regs.PRD.all = 0xFFFFFFFF;     //设置定时器0的周期寄存器值
    CpuTimer0Regs.TPR.all = 0;
    CpuTimer0Regs.TPRH.all = 0;             //设置定时器预定标计数器值为0

    CpuTimer0Regs.TCR.bit.TSS = 1;          //确保定时器0为停止状态
 
    CpuTimer0Regs.TCR.bit.TRB = 1;          //重载使能

    CpuTimer0.InterruptCount = 0;           //重置中断计数器

    ConfigCpuTimer(&CpuTimer0, Freq, Period);

    EALLOW;
    PieVectTable.TINT0 = &Timer0_TRQn;
    EDIS;

    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

    CpuTimer0Regs.TCR.bit.TSS = 0;

    IER |= M_INT1;
    EINT;
    ERTM;
}

void Timer1_Init(float Freq, float Period)
{
    EALLOW;
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER1ENCLK = 1;
    EDIS;

    CpuTimer1.RegsAddr = &CpuTimer1Regs;    //指向定时器1的寄存器地址，这句话是用来供通用函数使用，例如ConfigCpuTimer(&CpuTimer0, Freq, Period)

    CpuTimer1Regs.PRD.all = 0xFFFFFFFF;     //设置定时器1的周期寄存器值
    CpuTimer1Regs.TPR.all = 0;
    CpuTimer1Regs.TPRH.all = 0;             //设置定时器预定标计数器值为0

    CpuTimer1Regs.TCR.bit.TSS = 1;          //确保定时器1为停止状态
 
    CpuTimer1Regs.TCR.bit.TRB = 1;          //重载使能

    CpuTimer1.InterruptCount = 0;           //重置中断计数器

    ConfigCpuTimer(&CpuTimer1, Freq, Period);

    EALLOW;
    PieVectTable.XINT13 = &Timer1_TRQn;
    EDIS;

    CpuTimer1Regs.TCR.bit.TSS = 0;

    IER |= M_INT13;
    EINT;
    ERTM;
}

interrupt void Timer0_TRQn(void)
{
    EALLOW;
    Led2_Toggle;;
    PieCtrlRegs.PIEACK.bit.ACK1 = 1;
    EDIS;
}

interrupt void Timer1_TRQn(void)
{
	Uint16 Tx_Data = 0;
	Uint16 Rx_Data = 0;
	Uint16 Tx_data[4];
	Uint16 Rx_data[4];

	float ADC_Vol;

    Tx_Data = 20;
	Tx_data[0] = 20;
	Tx_data[1] = 30;
	Tx_data[2] = 40;
	Tx_data[3] = 50;
	SPI_FIFO_Transfer16(&Tx_Data, &Rx_Data, 1);
	SPI_FIFO_Transfer16(Tx_data, Rx_data, 4);

    ADC_Vol = (float)Read_ADC_Value()*3.0/4095;
    LCD9648_ShowString(0U, 0U, "Motor Demo");
	LCD9648_ShowString(0U, 1U, "Voltage:");
	LCD9648_ShowString(0U, 0U, "Motor Demo");
	LCD9648_ShowString(0U, 1U, "Voltage:");
	LCD9648_ShowDecimal(48U, 1U, ADC_Vol, 1U, 2U);
	LCD9648_ShowNumber(0U, 2U, Rx_Data, 8U);
	LCD9648_ShowNumber(0U, 3U, Rx_data[0], 2U);
	LCD9648_ShowNumber(18U, 3U, Rx_data[1], 2U);
	LCD9648_ShowNumber(36U, 3U, Rx_data[2], 2U);
	LCD9648_ShowNumber(54U, 3U, Rx_data[3], 2U);
}
