#include "Relay.h"

void Relay_Init(void)
{
    EALLOW;
    GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO15 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0;
    EDIS;
    GpioDataRegs.GPACLEAR.bit.GPIO15 = 1;
}
