#include "ePWM.h"

void ePWM6_Init(Uint16 tbprd)
{
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC=0; //关闭已使能的 ePWM 模块的时基时钟
    SysCtrlRegs.PCLKCR1.bit.EPWM6ENCLK=1; //ePWM1
    EDIS;

    InitEPwm6Gpio();

    //TB
    EPwm6Regs.TBCTL.bit.SYNCOSEL=TB_SYNC_DISABLE; //禁止同步信号输入
    EPwm6Regs.TBCTL.bit.PHSEN=TB_DISABLE;//当同步信号到来时，计数寄存器装载相位寄存器的值
    EPwm6Regs.TBPHS.half.TBPHS=0;//只有在PHSEN使能才有用
    EPwm6Regs.TBCTR=0x0000; //清除时基寄存器
    EPwm6Regs.TBPRD=tbprd;
    EPwm6Regs.TBCTL.bit.CTRMODE=TB_COUNT_UP; //向上计数
    EPwm6Regs.TBCTL.bit.HSPCLKDIV=TB_DIV1;
    EPwm6Regs.TBCTL.bit.CLKDIV=TB_DIV1;

    //SCMPA
    EPwm6Regs.CMPCTL.bit.SHDWAMODE=CC_SHADOW;//影子模式
    EPwm6Regs.CMPCTL.bit.SHDWBMODE=CC_SHADOW;//影子模式
    EPwm6Regs.CMPCTL.bit.LOADAMODE=CC_CTR_ZERO;
    EPwm6Regs.CMPCTL.bit.LOADBMODE=CC_CTR_ZERO;
    EPwm6Regs.CMPA.half.CMPA=0; //SetcompareAvalue
    EPwm6Regs.CMPB=0; //SetCompareBvalue

    //AQ
    EPwm6Regs.AQCTLA.bit.ZRO=AQ_CLEAR; //SetPWM1AonZero
    EPwm6Regs.AQCTLA.bit.CAU=AQ_SET; //ClearPWM1AoneventA,upcount
    EPwm6Regs.AQCTLB.bit.ZRO=AQ_CLEAR; //SetPWM1BonZero
    EPwm6Regs.AQCTLB.bit.CBU=AQ_SET; //ClearPWM1BoneventB,upcount

    //ET
    EPwm6Regs.ETSEL.bit.INTSEL=ET_CTR_ZERO; //TBCTR = 0时产生中断
    EPwm6Regs.ETSEL.bit.INTEN=1; //使能中断
    EPwm6Regs.ETPS.bit.INTPRD=ET_1ST; //每次发送事件时，都产生SOC信号

    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC=1; //开启所有已使能的 ePWM 模块的时基时钟
    EDIS;
}

void ePwm6A_SetCompare(Uint16 val)
{
	EPwm6Regs.CMPA.half.CMPA = val;  //设置占空比
}

void ePwm6B_SetCompare(Uint16 val)
{
	EPwm6Regs.CMPB = val;  //设置占空比
}
