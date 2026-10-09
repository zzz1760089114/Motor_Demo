#include "ADC.h"

void ADC_Init(void)
{
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;
    SysCtrlRegs.HISPCP.all = ADC_MODCLK;    //3
    EDIS;

    InitAdc();

    AdcRegs.ADCTRL3.bit.ADCCLKPS = ADC_CKPS;    //0x1;1
    AdcRegs.ADCTRL1.bit.CPS = 0;
    AdcRegs.ADCTRL1.bit.ACQ_PS = ADC_SHCLK;     //0xf;15
    AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;
    AdcRegs.ADCTRL1.bit.CONT_RUN = 1;
    
    AdcRegs.ADCTRL2.all = 0x2000;

    AdcRegs.ADCMAXCONV.bit.MAX_CONV1 = 0x0;
    AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;
}

Uint16 Read_ADC_Value(void)
{
    while (AdcRegs.ADCST.bit.INT_SEQ1 == 0)
    {
        //等待ADC转换完成
    }
    AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;
    return AdcRegs.ADCRESULT0>>4;
}
