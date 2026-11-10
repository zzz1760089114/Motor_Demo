#ifndef SCI_H
#define SCI_H

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

void SCIA_EnableFIFO_Init(Uint32 baud);
void SCIA_FIFO_Init(Uint32 baud);
void SCI_A_SendChar(Uint16 data);
Uint16 SCI_A_ReadChar(void);
Uint16 SCI_Transfer16(Uint16 tx);
Uint16 SCI_FIFO_Transfer16(const Uint16 *tx, Uint16 *rx, Uint16 count);

#endif
