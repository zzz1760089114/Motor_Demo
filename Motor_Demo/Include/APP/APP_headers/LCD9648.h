#ifndef LCD9648_H_
#define LCD9648_H_

#include "DSP2833x_Device.h"
#include "DSP2833x_Examples.h"

/* LCD9648 (UC1701) serial signals connected through the LCD12864 P2 header:
 *
 *   LCD9648 P1-4 SCL   -> P2-4 (GPIO0)
 *   LCD9648 P1-5 SDA   -> P2-5 (GPIO60)
 *   LCD9648 P1-6 RS/A0 -> P2-6 (GPIO1)
 *   LCD9648 P1-7 CS    -> P2-7 (GPIO2)
 *   LCD9648 P1-8 RESET -> P2-8 (GPIO3)
 *
 * P1-1 is GND, P1-2 must be powered from 3.3 V, and P1-3 is not connected.
 */
#define LCD9648_SCL_HIGH()    (GpioDataRegs.GPASET.bit.GPIO0 = 1)
#define LCD9648_SCL_LOW()     (GpioDataRegs.GPACLEAR.bit.GPIO0 = 1)

#define LCD9648_SDA_HIGH()    (GpioDataRegs.GPBSET.bit.GPIO60 = 1)
#define LCD9648_SDA_LOW()     (GpioDataRegs.GPBCLEAR.bit.GPIO60 = 1)

#define LCD9648_RS_HIGH()     (GpioDataRegs.GPASET.bit.GPIO1 = 1)
#define LCD9648_RS_LOW()      (GpioDataRegs.GPACLEAR.bit.GPIO1 = 1)

#define LCD9648_CS_HIGH()     (GpioDataRegs.GPASET.bit.GPIO2 = 1)
#define LCD9648_CS_LOW()      (GpioDataRegs.GPACLEAR.bit.GPIO2 = 1)

#define LCD9648_RESET_HIGH()  (GpioDataRegs.GPASET.bit.GPIO3 = 1)
#define LCD9648_RESET_LOW()   (GpioDataRegs.GPACLEAR.bit.GPIO3 = 1)

#define LCD9648_WIDTH         96U
#define LCD9648_HEIGHT        48U
#define LCD9648_PAGE_COUNT    (LCD9648_HEIGHT / 8U)

void LCD9648_GPIO_Init(void);
void LCD9648_WriteCmd(Uint16 cmd);
void LCD9648_WriteData(Uint16 data);
void LCD9648_Init(void);
void LCD9648_SetCursor(Uint16 x, Uint16 page);
void LCD9648_Clear(void);
void LCD9648_Fill(Uint16 pattern);
void LCD9648_SetContrast(Uint16 contrast);
void LCD9648_DisplayOn(void);
void LCD9648_DisplayOff(void);
void LCD9648_DrawBitmap(const Uint16 *bitmap, Uint16 size);
void LCD9648_ShowString(Uint16 x, Uint16 page, const char *text);
void LCD9648_ShowNumber(Uint16 x, Uint16 page, Uint32 value,
                        Uint16 fieldDigits);
void LCD9648_ShowDecimal(Uint16 x, Uint16 page, float value,
                         Uint16 integerDigits, Uint16 decimalPlaces);

#endif
