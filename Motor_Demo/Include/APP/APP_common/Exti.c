#include "Exti.h"
#include "Relay.h"
#include "Led.h"
#include "SCI.h"

#define KEY1_DEBOUNCE_SAMPLES    2U
#define KEY1_STATE_IDLE          0U
#define KEY1_STATE_PRESS_CHECK   1U
#define KEY1_STATE_WAIT_RELEASE  2U

static volatile Uint16 key1State;
static volatile Uint16 key1StableCount;
volatile Uint16 Key_Tx_Count;
volatile Uint16 Key_Rx_Count;

void EXTI1_Init(void)
{
    EALLOW;
    SysCtrlRegs.PCLKCR3.bit.GPIOINENCLK = 1;
    EDIS;

    EALLOW;
    GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO12 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0;
    GpioCtrlRegs.GPAQSEL1.bit.GPIO12 = 0;
    EDIS;

    EALLOW;
    GpioCtrlRegs.GPBMUX2.bit.GPIO48 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO48 = 1;
    GpioCtrlRegs.GPBPUD.bit.GPIO48 = 0;
    GpioDataRegs.GPBCLEAR.bit.GPIO48 = 1;
    EDIS;

    EALLOW;
    GpioIntRegs.GPIOXINT1SEL.bit.GPIOSEL = 12;
    EDIS;

    EALLOW;
    PieVectTable.XINT1 = &EXTI1_IRQn;
    EDIS;

    PieCtrlRegs.PIEIER1.bit.INTx4 = 1;
    
    // 初始化按键状态机
    key1State = KEY1_STATE_IDLE;
    key1StableCount = 0U;
    Key_Tx_Count = 0U;
    Key_Rx_Count = 0U;

    XIntruptRegs.XINT1CR.bit.POLARITY = 0;
    XIntruptRegs.XINT1CR.bit.ENABLE = 1;

    IER |= M_INT1;
    EINT;
    ERTM;
}

interrupt void EXTI1_IRQn(void)
{
    XIntruptRegs.XINT1CR.bit.ENABLE = 0;
    key1StableCount = 0U;
    key1State = KEY1_STATE_PRESS_CHECK;
    PieCtrlRegs.PIEACK.bit.ACK1 = 1;
}

void KEY1_DebounceTask(void)
{
    Uint16 txCount;
    Uint16 rxCount;

    if (key1State == KEY1_STATE_PRESS_CHECK)
    {
        if (KEY_H1 == 0U)
        {
            key1StableCount++;
            if (key1StableCount >= KEY1_DEBOUNCE_SAMPLES)
            {
                Key_Tx_Count++;
                Led1_Toggle;
                txCount = Key_Tx_Count;
                if (SCI_FIFO_Transfer16(&txCount, &rxCount, 1U) != 0U)
                {
                    /* 完整接收后再更新共享变量 */
                    Key_Rx_Count = rxCount;
                }
                key1StableCount = 0U;
                key1State = KEY1_STATE_WAIT_RELEASE;
            }
        }
        else
        {
            key1StableCount = 0U;
            key1State = KEY1_STATE_IDLE;
            XIntruptRegs.XINT1CR.bit.ENABLE = 1;
        }
    }
    else if (key1State == KEY1_STATE_WAIT_RELEASE)
    {
        if (KEY_H1 != 0U)
        {
            key1StableCount++;
            if (key1StableCount >= KEY1_DEBOUNCE_SAMPLES)
            {
                key1StableCount = 0U;
                key1State = KEY1_STATE_IDLE;
                XIntruptRegs.XINT1CR.bit.ENABLE = 1;
            }
        }
        else
        {
            key1StableCount = 0U;
        }
    }
}
