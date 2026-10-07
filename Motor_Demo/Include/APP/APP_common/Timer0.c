#include "Timer0.h"
#include "Led.h"

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

interrupt void Timer0_TRQn(void)
{
    EALLOW;
    Led2_Toggle;;
    PieCtrlRegs.PIEACK.bit.ACK1 = 1;
    EDIS;
}
