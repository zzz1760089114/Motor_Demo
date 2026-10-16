#include "SPI.h"

void SPIA_Init(void)
{
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.SPIAENCLK = 1;
    EDIS;

    InitSpiGpio();

    //Initialize SPI FIFO registers
    // SpiaRegs.SPIFFTX.all = 0xE040;
    // SpiaRegs.SPIFFRX.all = 0x204f;
    // SpiaRegs.SPIFFCT.all = 0x0;
    // SpiaRegs.SPICCR.all = 0x000F;
    // SpiaRegs.SPICTL.all = 0x0006;
    // SpiaRegs.SPIBRR = 0x007F;
    // SpiaRegs.SPICCR.all = 0x009F;
    // SpiaRegs.SPIPRI.bit.FREE = 1;

    SpiaRegs.SPICCR.all = 0;
    SpiaRegs.SPICTL.all = 0;

    SpiaRegs.SPICCR.bit.SPICHAR = 7;       // 8位
    SpiaRegs.SPICCR.bit.CLKPOLARITY = 0;   // 时钟空闲低
    SpiaRegs.SPICCR.bit.SPILBK = 1;        // 内部回环测试

    SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;  // 主机
    SpiaRegs.SPICTL.bit.TALK = 1;          // 允许发送
    SpiaRegs.SPICTL.bit.CLK_PHASE = 1;     // 对应通用Mode 0

    SpiaRegs.SPIBRR = 37;
   
    SpiaRegs.SPIFFTX.bit.SPIFFENA = 0;      // 关闭 FIFO 增强功能 

    SpiaRegs.SPIPRI.bit.FREE = 1;           // 调试器暂停时允许 SPI 继续运行

    SpiaRegs.SPICCR.bit.SPISWRESET = 1;     // 配置完成，释放复位
}

Uint16 SPI_Transfer8(Uint16 tx)
{
    // 等待发送缓冲区有空位
    while (SpiaRegs.SPISTS.bit.BUFFULL_FLAG != 0)
    {
    }

    // 8位发送数据左对齐
    SpiaRegs.SPITXBUF = (tx & 0x00FF) << 8;

    // 等待这一份数据收发完成
    // 实际项目可加入超时处理
    while (SpiaRegs.SPISTS.bit.INT_FLAG == 0)
    {
    }

    // 接收数据右对齐，读取后清除完成标志
    return SpiaRegs.SPIRXBUF & 0x00FF;
}

Uint16 SPIA_SendReciveData(Uint16 dat)
{
    //Transmitdata
    SpiaRegs.SPITXBUF = dat;    //发送16位的数据，要左对齐；例如配置为8位，想发送0xA5:  1、SpiaRegs.SPITXBUF = 0xA500;   2、SpiaRegs.SPITXBUF = ((Uint16)0xA5) << 8;
    //Waituntildataisreceived
    while(SpiaRegs.SPIFFRX.bit.RXFFST != 1)
    return SpiaRegs.SPIRXBUF;   //接收数据为右对齐
}
