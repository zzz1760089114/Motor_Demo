#include "SPI.h"

void SPIA_EnableFIFO_Init(void)
{
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.SPIAENCLK = 1;
    EDIS;

    InitSpiGpio();

    SpiaRegs.SPICCR.all = 0;
    SpiaRegs.SPICTL.all = 0;

    SpiaRegs.SPICCR.bit.SPICHAR = 15;     // 每个数据单元16位
    SpiaRegs.SPICCR.bit.CLKPOLARITY = 0;   // 时钟空闲低
    SpiaRegs.SPICCR.bit.SPILBK = 1;        // 内部回环测试,自己发送/自己接收

    SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;  // 主机
    SpiaRegs.SPICTL.bit.TALK = 1;          // 允许发送
    SpiaRegs.SPICTL.bit.CLK_PHASE = 1;     // 对应通用Mode 0

    SpiaRegs.SPIBRR = 37;                   //SPI波特率
   
    SpiaRegs.SPIFFTX.bit.SPIFFENA = 0;      // 关闭 FIFO 增强功能 
    SpiaRegs.SPIFFCT.all = 0;

    SpiaRegs.SPIPRI.bit.FREE = 1;           // 调试器暂停时允许 SPI 继续运行

    SpiaRegs.SPICCR.bit.SPISWRESET = 1;     // 配置完成，释放复位
}

void SPIA_FIFO_Init(void)
{
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.SPIAENCLK = 1;
    EDIS;
    
    InitSpiGpio();

    //SPI保持复位
    SpiaRegs.SPICCR.all = 0;
    SpiaRegs.SPICTL.all = 0;

    //基本通信参数
    SpiaRegs.SPICCR.bit.SPICHAR = 15;      // 16位
    SpiaRegs.SPICCR.bit.CLKPOLARITY = 0;   // 时钟空闲低
    SpiaRegs.SPICCR.bit.SPILBK = 1;        // 内部回环

    SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;  // 主机
    SpiaRegs.SPICTL.bit.TALK = 1;          // 允许发送
    SpiaRegs.SPICTL.bit.CLK_PHASE = 1;     // 通用Mode 0

    SpiaRegs.SPIBRR = 37;                   //SPI波特率

    //先让FIFO及收发通道保持复位
    SpiaRegs.SPIFFTX.all = 0;
    SpiaRegs.SPIFFRX.all = 0;
    SpiaRegs.SPIFFCT.all = 0;              // 无附加发送延迟

    //启用FIFO，采用轮询
    SpiaRegs.SPIFFTX.bit.SPIFFENA = 1;

    SpiaRegs.SPIFFTX.bit.TXFFIENA = 0;     // 不开TX FIFO中断
    SpiaRegs.SPIFFTX.bit.TXFFIL = 0;
    SpiaRegs.SPIFFTX.bit.TXFFINTCLR = 1;   // 清发送中断标志

    SpiaRegs.SPIFFRX.bit.RXFFIENA = 0;     // 不开RX FIFO中断
    SpiaRegs.SPIFFRX.bit.RXFFIL = 1;
    SpiaRegs.SPIFFRX.bit.RXFFINTCLR = 1;   // 清接收中断标志
    SpiaRegs.SPIFFRX.bit.RXFFOVFCLR = 1;   // 清接收溢出标志

    //释放收发通道和FIFO
    SpiaRegs.SPIFFTX.bit.SPIRST = 1;
    SpiaRegs.SPIFFTX.bit.TXFIFO = 1;
    SpiaRegs.SPIFFRX.bit.RXFIFORESET = 1;

    SpiaRegs.SPIPRI.bit.FREE = 1;

    //最后释放SPI复位
    SpiaRegs.SPICCR.bit.SPISWRESET = 1;
}

// 内部发送接收
Uint16 SPI_Transfer16(Uint16 tx)
{
    // 等待发送缓冲区有空位
    while (SpiaRegs.SPISTS.bit.BUFFULL_FLAG != 0)
    {
    }

    // 16位模式直接写入完整数据，不需要左移
    SpiaRegs.SPITXBUF = tx;

    // 等待这一份数据收发完成
    // 实际项目可加入超时处理
    while (SpiaRegs.SPISTS.bit.INT_FLAG == 0)
    {
    }

    // 读取完整的16位接收数据，同时清除完成标志
    return SpiaRegs.SPIRXBUF;
}

// 返回1：完成；返回0：数量不合法  内部发送接收
Uint16 SPI_FIFO_Transfer16(const Uint16 *tx, Uint16 *rx, Uint16 count)
{
    Uint16 i;

    if ((count == 0) || (count > 16))
    {
        return 0;
    }

    //连续向发送FIFO放入数据
    for (i = 0; i < count; i++)
    {
        // 16表示FIFO已满
        while (SpiaRegs.SPIFFTX.bit.TXFFST >= 16)
        {
        }

        // 16位模式直接写入完整数据
        SpiaRegs.SPITXBUF = tx[i];
    }

    //按接收顺序取出相同数量的数据
    for (i = 0; i < count; i++)
    {
        // 0表示暂时没有收到数据
        while (SpiaRegs.SPIFFRX.bit.RXFFST == 0)
        {
        }

        // 读取完整的16位接收数据
        rx[i] = SpiaRegs.SPIRXBUF;
    }

    SpiaRegs.SPIFFRX.bit.RXFFINTCLR = 1;
    SpiaRegs.SPIFFRX.bit.RXFFOVFCLR = 1;

    return 1;
}
