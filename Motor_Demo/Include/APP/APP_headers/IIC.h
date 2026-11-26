#ifndef IIC_H
#define IIC_H

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

#define IIC_SDA_SETH    (GpioDataRegs.GPBSET.bit.GPIO32 = 1)
#define IIC_SDA_SETL    (GpioDataRegs.GPBCLEAR.bit.GPIO32 = 1)
#define IIC_SCL_SETH    (GpioDataRegs.GPBSET.bit.GPIO33 = 1)
#define IIC_SCL_SETL    (GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1)
#define READ_SDA   		(GpioDataRegs.GPBDAT.bit.GPIO32)

#define AT24C01     127
#define AT24C02     255
#define AT24C04     511
#define AT24C08     1023
#define AT24C16     2047
#define AT24C32     4095
#define AT24C64     8191
#define AT24C128    16383
#define AT24C256    32767
#define EE_TYPE     AT24C02

void IIC_Init(void);
void SDA_OUT(void);
void SDA_IN(void);
void IIC_Start(void);
void IIC_Stop(void);
unsigned char IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);
void IIC_Send_Byte(unsigned char txd);
unsigned char IIC_Read_Byte(unsigned char ack);
unsigned char AT24CXX_ReadOneByte(Uint16 ReadAddr);
void AT24CXX_WriteOneByte(Uint16 WriteAddr,unsigned char DataToWrite);
void AT24CXX_WriteLenByte(Uint16 WriteAddr,Uint32 DataToWrite,unsigned char Len);
Uint32 AT24CXX_ReadLenByte(Uint16 ReadAddr,unsigned char Len);
unsigned char AT24CXX_Check(void);
void AT24CXX_Read(Uint16 ReadAddr,unsigned char *pBuffer,Uint16 NumToRead);
void AT24CXX_Write(Uint16 WriteAddr,unsigned char *pBuffer,Uint16 NumToWrite);

#endif
