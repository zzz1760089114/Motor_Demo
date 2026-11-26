#include "Key.h"
#include "Led.h"
#include "SCI.h"
#include "IIC.h"

#define KEY_STATE_IDLE          0U
#define KEY_STATE_PRESS_CHECK   1U
#define KEY_STATE_WAIT_RELEASE  2U
#define KEY_MATRIX_MULTIPLE     0xFFFFU

static Uint16 keyState;
static Uint16 keyStableCount;
static Uint16 keyCandidate;
static Uint16 keyPending;

volatile Uint16 Key_Matrix_LastKey;
volatile Uint32 Key_Matrix_Count;

volatile Uint16 Key_Tx_Count;
volatile Uint16 Key_Rx_Count;

volatile Uint16 K;

/* Unselected columns are inputs, never driven high. */
static void KEY_ReleaseColumns(void)
{
    EALLOW;
    GpioCtrlRegs.GPBDIR.bit.GPIO48 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO49 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO50 = 0;
    EDIS;
}

static Uint16 KEY_ReadRaw(void)
{
    Uint16 column;
    Uint16 row;
    Uint16 rows;
    Uint16 key;
    Uint16 result = KEY_MATRIX_NONE;

    for (column = 0U; column < 3U; column++)
    {
        KEY_ReleaseColumns();
        EALLOW;
        if (column == 0U)
        {
            GpioCtrlRegs.GPBDIR.bit.GPIO48 = 1;
        }
        else if (column == 1U)
        {
            GpioCtrlRegs.GPBDIR.bit.GPIO49 = 1;
        }
        else
        {
            GpioCtrlRegs.GPBDIR.bit.GPIO50 = 1;
        }
        EDIS;

        /* Allow the external pull-ups and input synchronizers to settle. */
        DELAY_US(5);
        rows = (Uint16)((GpioDataRegs.GPADAT.all >> 12) & 0x7UL);
        for (row = 0U; row < 3U; row++)
        {
            if ((rows & (1U << row)) == 0U)
            {
                key = row * 3U + column + 1U;
                if (result == KEY_MATRIX_NONE)
                {
                    result = key;
                }
                else
                {
                    result = KEY_MATRIX_MULTIPLE;
                }
            }
        }
    }

    KEY_ReleaseColumns();
    return result;
}

void KEY_Matrix_Init(void)
{
    EALLOW;
    SysCtrlRegs.PCLKCR3.bit.GPIOINENCLK = 1;

    GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO12 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO13 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO14 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO13 = 0;
    GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0;
    GpioCtrlRegs.GPAQSEL1.bit.GPIO12 = 0;
    GpioCtrlRegs.GPAQSEL1.bit.GPIO13 = 0;
    GpioCtrlRegs.GPAQSEL1.bit.GPIO14 = 0;

  
    GpioCtrlRegs.GPBDIR.bit.GPIO48 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO49 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO50 = 0;
    GpioDataRegs.GPBCLEAR.bit.GPIO48 = 1;
    GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1;
    GpioDataRegs.GPBCLEAR.bit.GPIO50 = 1;
    GpioCtrlRegs.GPBMUX2.bit.GPIO48 = 0;
    GpioCtrlRegs.GPBMUX2.bit.GPIO49 = 0;
    GpioCtrlRegs.GPBMUX2.bit.GPIO50 = 0;
    GpioCtrlRegs.GPBPUD.bit.GPIO48 = 1;
    GpioCtrlRegs.GPBPUD.bit.GPIO49 = 1;
    GpioCtrlRegs.GPBPUD.bit.GPIO50 = 1;
    EDIS;

    keyState = KEY_STATE_IDLE;
    keyStableCount = 0U;
    keyCandidate = KEY_MATRIX_NONE;
    keyPending = KEY_MATRIX_NONE;
    Key_Matrix_LastKey = KEY_MATRIX_NONE;
    Key_Matrix_Count = 0UL;

    Key_Tx_Count = 0;
    Key_Rx_Count = 0;

    K = 0;
}

void KEY_Matrix_ScanTask(void)
{
    Uint16 rawKey = KEY_ReadRaw();

    switch (keyState)
    {
        case KEY_STATE_IDLE:        //空闲状态，寻找按键
            if (rawKey == KEY_MATRIX_MULTIPLE)
            {
                keyStableCount = 0U;    //清零消抖次数
                keyState = KEY_STATE_WAIT_RELEASE;
            }
            else if (rawKey != KEY_MATRIX_NONE)
            {
                keyCandidate = rawKey;
                keyStableCount = 0U;
                keyState = KEY_STATE_PRESS_CHECK;
            }
            break;

        case KEY_STATE_PRESS_CHECK:
            if (rawKey == keyCandidate)
            {
                keyStableCount++;
                if (keyStableCount >= KEY_MATRIX_DEBOUNCE_SAMPLES)
                {
                    /* ProcessEvent consumes this before the next scan. */
                    if (keyPending == KEY_MATRIX_NONE)
                    {
                        keyPending = keyCandidate;
                    }
                    keyStableCount = 0U;
                    keyState = KEY_STATE_WAIT_RELEASE;
                }
            }
            else
            {
                keyStableCount = 0U;
                keyCandidate = KEY_MATRIX_NONE;
                keyState = (rawKey == KEY_MATRIX_NONE) ?
                           KEY_STATE_IDLE : KEY_STATE_WAIT_RELEASE;
            }
            break;

        case KEY_STATE_WAIT_RELEASE:
            if (rawKey == KEY_MATRIX_NONE)
            {
                keyStableCount++;
                if (keyStableCount >= KEY_MATRIX_DEBOUNCE_SAMPLES)
                {
                    keyStableCount = 0U;
                    keyCandidate = KEY_MATRIX_NONE;
                    keyState = KEY_STATE_IDLE;
                }
            }
            else
            {
                keyStableCount = 0U;
            }
            break;

        default:
            keyStableCount = 0U;
            keyCandidate = KEY_MATRIX_NONE;
            keyState = KEY_STATE_IDLE;
            break;
    }
}

void KEY_ProcessEvent(void)
{
    Uint16 key = keyPending;
    Uint16 txCount;
    Uint16 rxCount;

    if (key == KEY_MATRIX_NONE)
    {
        return;
    }
    keyPending = KEY_MATRIX_NONE;
    Key_Matrix_LastKey = key;
    Key_Matrix_Count++;

    switch (key)
    {
        case 1U: /* K1 */
            /* Original K1 action; counters remain defined in Exti.c. */
            Key_Tx_Count++;
            Led1_Toggle;
            txCount = Key_Tx_Count;
            SCI_FIFO_SendString("Key_Count:");
            DELAY_US(1000);
            Clear_FIFO();
            if (SCI_FIFO_Transfer16(&txCount, &rxCount, 1U) != 0U)
            {
                Key_Rx_Count = rxCount;
            }
            SCI_FIFO_SendString("\r\n");
            DELAY_US(1000);
            Clear_FIFO();
            break;

        case 2U: /* K2 */
            /* TODO: Add K2 action here. */
            
            break;

        case 3U: /* K3 */
            /* TODO: Add K3 action here. */
            
            break;

        case 4U: /* K4 */
            /* TODO: Add K4 action here. */
            K++;
            if(K>255)
                K=255;
            AT24CXX_WriteOneByte(0, K);
            break;

        case 5U: /* K5 */
            /* TODO: Add K5 action here. */
            AT24CXX_ReadOneByte(0);
            break;

        case 6U: /* K6 */
            /* TODO: Add K6 action here. */
            K = 0;
            break;

        case 7U: /* K7 */
            /* TODO: Add K7 action here. */

            break;

        case 8U: /* K8 */
            /* TODO: Add K8 action here. */

            break;

        case 9U: /* K9 */
            /* TODO: Add K9 action here. */

            break;

        default:
            break;
    }

}
