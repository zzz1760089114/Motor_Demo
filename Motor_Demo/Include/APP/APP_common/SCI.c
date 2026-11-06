#include "SCI.h"

void SCIA_EnableFIFO_Init(Uint32 baud)
{
    unsigned char scihbaud = 0;
    unsigned char scilbaud = 0;
    Uint16 scibaud = 0;

    scibaud = 37500000 / ( 8 * baud ) - 1;
    scihbaud = scibaud >> 8;
    scilbaud = scibaud & 0xff;

    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.SCIAENCLK = 1;
    EDIS;

    EALLOW;
    GpioCtrlRegs.GPBPUD.bit.GPIO35 = 0;	 //Enable pull-up for GPIO35 (SCITXDA)
    GpioCtrlRegs.GPBPUD.bit.GPIO36 = 0;	 //Enable pull-up for GPIO36 (SCIRXDA)

    GpioCtrlRegs.GPBQSEL1.bit.GPIO36 = 3;  // Asynch input GPIO36 (SCIRXDA)

    GpioCtrlRegs.GPBMUX1.bit.GPIO35 = 1;  //Configure GPIO35 to SCITXDA
    GpioCtrlRegs.GPBMUX1.bit.GPIO36 = 1;   //Configure GPIO36 for SCIRXDA
    EDIS;

    // 保持软件复位
    SciaRegs.SCICTL1.all = 0;
    SciaRegs.SCICCR.all = 0;

    SciaRegs.SCICCR.bit.SCICHAR = 7;
    SciaRegs.SCICCR.bit.ADDRIDLE_MODE = 0;
    SciaRegs.SCICCR.bit.LOOPBKENA = 1;
    SciaRegs.SCICCR.bit.PARITYENA = 0;
    SciaRegs.SCICCR.bit.STOPBITS = 0;

    SciaRegs.SCICTL1.bit.SWRESET = 0;
    SciaRegs.SCICTL1.bit.RXENA = 1;
    SciaRegs.SCICTL1.bit.TXENA = 1;
    SciaRegs.SCICTL1.bit.SWRESET = 1;

    SciaRegs.SCIHBAUD = scihbaud;
    SciaRegs.SCILBAUD = scilbaud;

    SciaRegs.SCIFFTX.bit.SCIFFENA = 0;      // 关闭 FIFO 增强功能

    // 允许发送和接收
    SciaRegs.SCICTL1.bit.TXENA = 1;
    SciaRegs.SCICTL1.bit.RXENA = 1;

    // 释放复位
    SciaRegs.SCICTL1.bit.SWRESET = 1;
}
 
void SCI_A_SendChar(Uint16 data)
{
    while (SciaRegs.SCICTL2.bit.TXRDY == 0)
    {
    }

    SciaRegs.SCITXBUF = data & 0x00FF;
}

Uint16 SCI_A_ReadChar(void)
{
    while (SciaRegs.SCIRXST.bit.RXRDY == 0)
    {
    }

    return SciaRegs.SCIRXBUF.all & 0x00FF;
}

Uint16 SCI_Transfer16(Uint16 tx)
{
    // 等待发送缓冲区有空位
    while (SciaRegs.SCICTL2.bit.TXRDY == 0)
    {
    }

    SciaRegs.SCITXBUF = tx & 0x00FF;

    // 等待这一份数据收发完成
    // 实际项目可加入超时处理
    while (SciaRegs.SCIRXST.bit.RXRDY == 0)
    {
    }

    return SciaRegs.SCIRXBUF.all & 0x00FF;
}
