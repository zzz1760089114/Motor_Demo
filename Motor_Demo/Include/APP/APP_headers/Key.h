#ifndef APP_MATRIX_KEY_H
#define APP_MATRIX_KEY_H

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

#define KEY_MATRIX_NONE              0U
#define KEY_MATRIX_DEBOUNCE_SAMPLES  2U

/* Matrix statistics: last processed key (1..9) and total press count. */
extern volatile Uint16 Key_Matrix_LastKey;
extern volatile Uint32 Key_Matrix_Count;

extern volatile Uint16 Key_Tx_Count;
extern volatile Uint16 Key_Rx_Count;

extern volatile Uint16 K;

/* Rows: GPIO12/13/14. Columns: GPIO48/49/50. */
void KEY_Matrix_Init(void);

/* Call once every 10 ms; scans all columns and advances the state machine.
 * Supports one key at a time. Multiple keys lock input until all are released.
 * Call ScanTask and ProcessEvent from the same main-loop context.
 */
void KEY_Matrix_ScanTask(void);

/* Call after each ScanTask, in main (SCI sending can block).
 * Requires Led_Init() and SCIA_FIFO_Init() to have completed.
 * K1 runs the original count/LED/SCI transfer action using the counters
 * defined in Exti.c. K2..K9 are reserved for user actions.
 */
void KEY_ProcessEvent(void);

#endif
