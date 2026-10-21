#ifndef SPI_H
#define SPI_H

#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

void SPIA_EnableFIFO_Init(void);
void SPIA_FIFO_Init(void);
Uint16 SPI_Transfer16(Uint16 tx);
Uint16 SPI_FIFO_Transfer16(const Uint16 *tx, Uint16 *rx, Uint16 count);

#endif
