#include "IIC.h"

void IIC_Init(void)
{
    EALLOW;
    SysCtrlRegs.PCLKCR3.bit.GPIOINENCLK = 1;
    EDIS;
    EALLOW;
    GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 0;
    GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO32 = 1;
    GpioCtrlRegs.GPBDIR.bit.GPIO33 = 1;
    GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO33 = 0;
    GpioCtrlRegs.GPBQSEL1.bit.GPIO32 = 3;
    GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 3;
    EDIS;
}

/****************************************************************
*函数名:SDA_OUT
*函数功能:SDA输出配置
*输入:无
*输出:无
*****************************************************************/
void SDA_OUT(void)
{
    EALLOW;
    GpioCtrlRegs.GPBDIR.bit.GPIO32=1;    //Output.SDA
    EDIS;
}

/****************************************************************
*函数名:SDA_IN
*函数功能:SDA输入配置
*输入:无
*输出:无
*****************************************************************/
void SDA_IN(void)
{
    EALLOW;
    GpioCtrlRegs.GPBDIR.bit.GPIO32=0;    //Input,SDA
    EDIS;
}


/****************************************************************
*函数名:IIC_Start
*函数功能:产生IIC起始信号
*输入:无
*输出:无
*****************************************************************/
void IIC_Start(void)
{
    SDA_OUT();                          //sda线输出
    IIC_SDA_SETH;
    IIC_SCL_SETH;
    DELAY_US(5);
    IIC_SDA_SETL;                       //START:whenCLKishigh,DATAchangeformhightolow
    DELAY_US(6);
    IIC_SCL_SETL;                       //钳住I2C总线，准备发送或接收数据
}

/****************************************************************
*函数名:IIC_Stop
*函数功能:产生IIC停止信号
*输入:无
*输出:无
*****************************************************************/
void IIC_Stop(void)
{
    SDA_OUT();                          //sda线输出
    IIC_SCL_SETL;
    IIC_SDA_SETL;                       //STOP:whenCLKishighDATAchangeformlowtohigh
    IIC_SCL_SETH;
    DELAY_US(6);
    IIC_SDA_SETH;                       //发送I2C总线结束信号
    DELAY_US(6);
}


/****************************************************************
*函数名:IIC_Wait_Ack
*函数功能:等待应答信号到来
*输入:无
*输出:1，接收应答失败
0，接收应答成功
*****************************************************************/
unsigned char IIC_Wait_Ack(void)
{
    unsigned char tempTime = 0;
    IIC_SDA_SETH;
    DELAY_US(1);
    SDA_IN();                           //SDA设置为输入
    IIC_SCL_SETH;
    DELAY_US(1);
    while(READ_SDA)
    {
        tempTime++;
        if(tempTime>250)
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL_SETL;//时钟输出0
    return 0;
}

/****************************************************************
*函数名:IIC_Ack
*函数功能:产生ACK应答
*输入:无
*输出:无
*****************************************************************/
void IIC_Ack(void)
{
    IIC_SCL_SETL;
    SDA_OUT();
    IIC_SDA_SETL;
    DELAY_US(2);
    IIC_SCL_SETH;
    DELAY_US(5);
    IIC_SCL_SETL;
}

/****************************************************************
*函数名:IIC_NAck
*函数功能:产生NACK非应答
*输入:无
*输出:无
*****************************************************************/
void IIC_NAck(void)
{
    IIC_SCL_SETL;
    SDA_OUT();
    IIC_SDA_SETH;
    DELAY_US(2);
    IIC_SCL_SETH;
    DELAY_US(5);
    IIC_SCL_SETL;
}

/****************************************************************
*函数名:IIC_Send_Byte
*函数功能:IIC发送一个字节
*输入:txd：发送一个字节
*输出:无
*****************************************************************/
void IIC_Send_Byte(unsigned char txd)
{
    unsigned char t;
    SDA_OUT();
    IIC_SCL_SETL;                       //拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {
        if((txd & 0x80) > 0)                //0x80 10000000
            IIC_SDA_SETH;
        else
            IIC_SDA_SETL;
        txd<<=1;
        DELAY_US(2);                    //对TEA5767这三个延时都是必须的
        IIC_SCL_SETH;
        DELAY_US(2);
        IIC_SCL_SETL;
        DELAY_US(2);
    }
}

/****************************************************************
*函数名:IIC_Read_Byte
*函数功能:IIC读一个字节
*输入:ack=1时，发送ACK，ack=0，发送nACK
*输出:应答或非应答
*****************************************************************/
unsigned char IIC_Read_Byte(unsigned char ack)
{
    unsigned char i,receive = 0;
    SDA_IN();                           //SDA设置为输入
    for(i=0;i<8;i++)
    {
        IIC_SCL_SETL;
        DELAY_US(2);
        IIC_SCL_SETH;
        receive<<=1;
        if(READ_SDA)
            receive++;
        DELAY_US(1);
    }
    if(!ack)
        IIC_NAck();                     //发送nACK
    else
        IIC_Ack();                      //发送ACK
    return receive;
}

/****************************************************************
*函数名:AT24CXX_ReadOneByte
*函数功能:在AT24CXX指定地址读出一个数据
*输入:ReadAddr:开始读数的地址
*输出:读到的数据
*****************************************************************/
unsigned char AT24CXX_ReadOneByte(Uint16 ReadAddr)
{
    unsigned char temp = 0;
    IIC_Start();
    if(EE_TYPE > AT24C16)
    {
        IIC_Send_Byte(0XA0);           //发送写命令
        IIC_Wait_Ack();
        IIC_Send_Byte(ReadAddr>>8);    //发送高地址
    }
    else
    {
        IIC_Send_Byte(0XA0+((ReadAddr/256)<<1)); //发送器件地址0XA0,写数据
    }
    IIC_Wait_Ack();
    IIC_Send_Byte(ReadAddr%256);       //发送低地址
    IIC_Wait_Ack();
    IIC_Start();
    IIC_Send_Byte(0XA1);               //进入接收模式
    IIC_Wait_Ack();
    temp = IIC_Read_Byte(0);
    IIC_Stop();                        //产生一个停止条件
    return temp;
}

/****************************************************************
*函数名:AT24CXX_WriteOneByte
*函数功能:在AT24CXX指定地址写入一个数据
*输入:WriteAddr  :写入数据的目的地址
      DataToWrite:要写入的数据
*输出:无
*****************************************************************/
void AT24CXX_WriteOneByte(Uint16 WriteAddr,unsigned char DataToWrite)
{
    IIC_Start();
    if(EE_TYPE>AT24C16)
    {
        IIC_Send_Byte(0XA0);            //发送写命令
        IIC_Wait_Ack();
        IIC_Send_Byte(WriteAddr>>8);    //发送高地址
    }
    else
    {
        IIC_Send_Byte(0XA0+((WriteAddr/256)<<1)); //发送器件地址0XA0,写数据
    }
    IIC_Wait_Ack();
    IIC_Send_Byte(WriteAddr%256);       //发送低地址
    IIC_Wait_Ack();
    IIC_Send_Byte(DataToWrite);         //发送字节
    IIC_Wait_Ack();
    IIC_Stop();                         //产生一个停止条件
    DELAY_US(10*1000);
}

/****************************************************************
*函数名:AT24CXX_WriteLenByte
*函数功能:在AT24CXX里面的指定地址开始写入长度为Len的数据用于写入16bit或者32bit的数据
*输入:WriteAddr  :写入数据的目的地址
      DataToWrite:要写入的数据
      Len :要写入数据的长度2,4
*输出:无
*****************************************************************/
void AT24CXX_WriteLenByte(Uint16 WriteAddr,Uint32 DataToWrite,unsigned char Len)
{
    unsigned char t;
    for(t=0;t<Len;t++)
    {
        AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
    }
}

/****************************************************************
*函数名:AT24CXX_ReadLenByte
*函数功能:在AT24CXX里面的指定地址开始读出长度为Len的数据用于读出16bit或者32bit的数据
*输入:ReadAddr :开始读出的地址
      Len      :要读出数据的长度2,4
*输出:读取的数据
*****************************************************************/
Uint32 AT24CXX_ReadLenByte(Uint16 ReadAddr,unsigned char Len)
{
    unsigned char t;
    Uint32 temp = 0;
    for(t=0;t<Len;t++)
    {
        temp<<=8;
        temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1);
    }
    return temp;
}


/****************************************************************
*函数名:AT24CXX_Check
*函数功能:检查AT24CXX是否正常
*输入:无
*输出:1:检测失败，0:检测成功
*****************************************************************/
unsigned char AT24CXX_Check(void)
{
    unsigned char temp;
    temp = AT24CXX_ReadOneByte(255);    //避免每次开机都写AT24CXX
    if(temp==0x36)
        return 0;
    else                                //排除第一次初始化的情况
    {
        AT24CXX_WriteOneByte(255,0X36);
        temp = AT24CXX_ReadOneByte(255);
        if(temp==0X36)
            return 0;
    }
    return 1;
}

/****************************************************************
*函数名:AT24CXX_Read
*函数功能:在AT24CXX里面的指定地址开始读出指定个数的数据
*输入:ReadAddr :开始读出的地址对24c02为0~255
      pBuffer  :数据数组首地址
      NumToRead:要读出数据的个数
*输出:无
*****************************************************************/
void AT24CXX_Read(Uint16 ReadAddr,unsigned char *pBuffer,Uint16 NumToRead)
{
    while(NumToRead)
    {
        *pBuffer ++= AT24CXX_ReadOneByte(ReadAddr++);
        NumToRead--;
    }
}

/****************************************************************
*函数名:AT24CXX_Write
*函数功能:在AT24CXX里面的指定地址开始写入指定个数的数据
*输入:WriteAddr :开始写入的地址对24c02为0~255
      pBuffer   :数据数组首地址
      NumToRead :要读出数据的个数
*输出:无
*****************************************************************/
void AT24CXX_Write(Uint16 WriteAddr,unsigned char *pBuffer,Uint16 NumToWrite)
{
    while(NumToWrite--)
    {
        AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
        WriteAddr++;
        pBuffer++;
    }
}
