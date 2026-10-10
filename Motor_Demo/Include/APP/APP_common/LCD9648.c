#include "LCD9648.h"
#include "Font.h"

/* UC1701 four-wire serial interface. Data is transferred MSB first and is
 * sampled by the LCD on each rising edge of SCL. */
static void LCD9648_SendByte(Uint16 value)  //底层串行发送函数
{
    Uint16 bit;

    for (bit = 0U; bit < 8U; bit++)
    {
        LCD9648_SCL_LOW();

        if ((value & 0x80U) != 0U)
        {
            LCD9648_SDA_HIGH();
        }
        else
        {
            LCD9648_SDA_LOW();
        }

        DELAY_US(1U);
        LCD9648_SCL_HIGH();
        DELAY_US(1U);
        value <<= 1U;
    }
}

/* Draw one 5x7 digit and one blank spacing column. */
static void LCD9648_DrawDigit(Uint16 x, Uint16 page, Uint16 digit)
{
    Uint16 column;

    if ((digit > 9U) || (x > (LCD9648_WIDTH - 5U)) ||
        (page >= LCD9648_PAGE_COUNT))
    {
        return;
    }

    LCD9648_SetCursor(x, page);

    for (column = 0U; column < 5U; column++)
    {
        LCD9648_WriteData(LCD9648_DigitFont[digit][column]);
    }

    if ((x + 5U) < LCD9648_WIDTH)
    {
        LCD9648_WriteData(0x00U);
    }
}

static void LCD9648_DrawDecimalPoint(Uint16 x, Uint16 page)
{
    Uint16 column;

    if ((x > (LCD9648_WIDTH - 2U)) ||
        (page >= LCD9648_PAGE_COUNT))
    {
        return;
    }

    LCD9648_SetCursor(x, page);

    for (column = 0U; column < 2U; column++)
    {
        LCD9648_WriteData(LCD9648_DecimalPointFont[column]);
    }

    if ((x + 2U) < LCD9648_WIDTH)
    {
        LCD9648_WriteData(0x00U);
    }
}

void LCD9648_WriteCmd(Uint16 cmd)
{
    LCD9648_CS_LOW();
    LCD9648_RS_LOW();   //RS = 0：发送命令；
    DELAY_US(1U);
    LCD9648_SendByte(cmd);
    LCD9648_CS_HIGH();
}

void LCD9648_WriteData(Uint16 data)
{
    LCD9648_CS_LOW();
    LCD9648_RS_HIGH();  //RS = 1：发送显示数据
    DELAY_US(1U);
    LCD9648_SendByte(data);
    LCD9648_CS_HIGH();
}

void LCD9648_GPIO_Init(void)
{
    EALLOW;

    SysCtrlRegs.PCLKCR3.bit.GPIOINENCLK = 1U;

    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0U;
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0U;
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0U;
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0U;
    GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 0U;

    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 0U;
    GpioCtrlRegs.GPAPUD.bit.GPIO1 = 0U;
    GpioCtrlRegs.GPAPUD.bit.GPIO2 = 0U;
    GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0U;
    GpioCtrlRegs.GPBPUD.bit.GPIO60 = 0U;

    GpioCtrlRegs.GPADIR.bit.GPIO0 = 1U;
    GpioCtrlRegs.GPADIR.bit.GPIO1 = 1U;
    GpioCtrlRegs.GPADIR.bit.GPIO2 = 1U;
    GpioCtrlRegs.GPADIR.bit.GPIO3 = 1U;
    GpioCtrlRegs.GPBDIR.bit.GPIO60 = 1U;

    EDIS;

    //设置安全空闲状态
    LCD9648_CS_HIGH();  //CS = 1：LCD没有被选中
    LCD9648_SCL_HIGH();
    LCD9648_SDA_HIGH();
    LCD9648_RS_LOW();
    LCD9648_RESET_HIGH();   //RESET = 1：复位无效
}

void LCD9648_Init(void)
{
    LCD9648_GPIO_Init();

    LCD9648_RESET_HIGH();
    DELAY_US(10000U);
    LCD9648_RESET_LOW();
    DELAY_US(10000U);
    LCD9648_RESET_HIGH();
    DELAY_US(10000U);

    LCD9648_WriteCmd(0xE2U); //软件复位
    LCD9648_WriteCmd(0xC8U); //设置COM扫描方向（COM对应液晶的行扫描方向）；如果上下颠倒，则改为0xC0U
    LCD9648_WriteCmd(0xA0U); //设置SEG方向（SEG对应水平方向的列）；如果左右镜像，则改为0xA1U
    LCD9648_WriteCmd(0x2FU); //打开内部电源电路
    LCD9648_WriteCmd(0x26U); //设置VLCD电阻比（考虑更改）
    LCD9648_SetContrast(0x15U);//设置对比度
    LCD9648_WriteCmd(0x40U); //设置起始行：表示显示从第0行开始
    LCD9648_WriteCmd(0xA6U); //表示正常显示（反相则设置为0xA7U）
    LCD9648_WriteCmd(0xA4U); //让屏幕按照显存显示
    LCD9648_DisplayOn();//打开显示
    LCD9648_Clear();//关闭显示
}

void LCD9648_SetCursor(Uint16 x, Uint16 page)//设置显示位置
{
    if ((x >= LCD9648_WIDTH) || (page >= LCD9648_PAGE_COUNT))//首先检查坐标是否合法
    {
        return;
    }

    LCD9648_WriteCmd(0xB0U | page);//设置页地址
    LCD9648_WriteCmd(0x10U | ((x >> 4U) & 0x0FU));//设置列地址（高4位低4位发送）
    LCD9648_WriteCmd(x & 0x0FU);
}

void LCD9648_Clear(void)//清屏
{
    LCD9648_Fill(0x00U);
}

void LCD9648_Fill(Uint16 pattern)//设置填充，0x00整屏熄灭，0xFF整屏点亮
{
    Uint16 page;
    Uint16 x;

    for (page = 0U; page < LCD9648_PAGE_COUNT; page++)
    {
        LCD9648_SetCursor(0U, page);
        for (x = 0U; x < LCD9648_WIDTH; x++)
        {
            LCD9648_WriteData(pattern);
        }
    }

    /* Clear the UC1701 COMS/icon row stored in display RAM page 8. */
    LCD9648_WriteCmd(0xB8U);
    LCD9648_WriteCmd(0x10U);
    LCD9648_WriteCmd(0x00U);

    for (x = 0U; x < 132U; x++)
    {
        LCD9648_WriteData(0x00U);
    }
}

void LCD9648_SetContrast(Uint16 contrast)//设置对比度
{
    LCD9648_WriteCmd(0x81U);//表示接下来设置的是对比度
    LCD9648_WriteCmd(contrast & 0x3FU);//设置具体值（对比度有效范围为6位，按位与可以清除高于第5位的内容）
}

void LCD9648_DisplayOn(void)
{
    LCD9648_WriteCmd(0xAFU);
}

void LCD9648_DisplayOff(void)
{
    LCD9648_WriteCmd(0xAEU);
}

void LCD9648_DrawBitmap(const Uint16 *bitmap, Uint16 size)//const表示函数只读取图片数据，不修改数组
{
    Uint16 page;
    Uint16 x;
    Uint16 index = 0U;

    if (bitmap == 0)
    {
        return;
    }

    for (page = 0U; page < LCD9648_PAGE_COUNT; page++)//图片显示
    {
        LCD9648_SetCursor(0U, page);
        for (x = 0U; x < LCD9648_WIDTH; x++)
        {
            if (index >= size)
            {
                return;
            }

            LCD9648_WriteData(bitmap[index]);
            index++;
        }
    }
}

void LCD9648_ShowString(Uint16 x, Uint16 page, const char *text)
{
    Uint16 character;
    Uint16 column;

    if ((text == 0) || (x >= LCD9648_WIDTH) ||
        (page >= LCD9648_PAGE_COUNT))
    {
        return;
    }

    while ((*text != '\0') && (x <= (LCD9648_WIDTH - 5U)))
    {
        character = (Uint16)(*text);

        if ((character < LCD9648_ASCII_FIRST_CHAR) ||
            (character > LCD9648_ASCII_LAST_CHAR))
        {
            character = (Uint16)'?';
        }

        LCD9648_SetCursor(x, page);
        for (column = 0U; column < 5U; column++)
        {
            LCD9648_WriteData(
                LCD9648_AsciiFont[character - LCD9648_ASCII_FIRST_CHAR]
                                   [column]);
        }

        if ((x + 5U) < LCD9648_WIDTH)
        {
            LCD9648_WriteData(0x00U);
        }

        x += 6U;
        text++;
    }
}

//显示整数:不要超出10位
void LCD9648_ShowNumber(Uint16 x, Uint16 page, Uint32 value, Uint16 fieldDigits)//（x位置,y位置,数值,位数）
{
    Uint16 digits[10];
    Uint16 digitCount = 0U;
    Uint16 clearColumns;
    Uint16 column;

    if ((x >= LCD9648_WIDTH) || (page >= LCD9648_PAGE_COUNT) ||
        (fieldDigits == 0U))
    {
        return;
    }

    if (fieldDigits > 10U)
    {
        fieldDigits = 10U;
    }

    /* Clear the complete fixed-width field before drawing the new value. */
    clearColumns = fieldDigits * 6U;
    if (clearColumns > (LCD9648_WIDTH - x))
    {
        clearColumns = LCD9648_WIDTH - x;
    }

    LCD9648_SetCursor(x, page);
    for (column = 0U; column < clearColumns; column++)
    {
        LCD9648_WriteData(0x00U);
    }

    /* Zero needs its own digit because the conversion loop is value > 0. */
    if (value == 0UL)
    {
        LCD9648_DrawDigit(x, page, 0U);
        return;
    }

    /* Decimal remainders arrive in reverse order: 123 -> 3, 2, 1. */
    while ((value > 0UL) && (digitCount < 10U))
    {
        digits[digitCount] = (Uint16)(value % 10UL);
        value /= 10UL;
        digitCount++;
    }

    /* Leave the cleared field blank when the value does not fit. */
    if (digitCount > fieldDigits)
    {
        return;
    }

    /* Read the temporary array backwards to restore the normal order. */
    while (digitCount > 0U)
    {
        digitCount--;

        if (x > (LCD9648_WIDTH - 5U))
        {
            return;
        }

        LCD9648_DrawDigit(x, page, digits[digitCount]);
        x += 6U;
    }
}

//显示小数:整数位数至少为 1，整数位数不能超过 9，小数位数不能超过 6，整数位数加小数位数不能超过 9
void LCD9648_ShowDecimal(Uint16 x, Uint16 page, float value, Uint16 integerDigits, Uint16 decimalPlaces)//（x位置,y位置,数值,整数位数（超出退出）,小数位数（超出四舍五入））
{
    Uint16 digits[10];
    Uint16 digitCount = 0U;
    Uint16 fieldColumns;
    Uint16 clearColumns;
    Uint16 column;
    Uint16 drawX;
    Uint16 index;
    Uint32 scale = 1UL;
    Uint32 scaledValue;
    Uint32 integerPart;
    Uint32 fractionPart;
    Uint32 divisor;

    if ((x >= LCD9648_WIDTH) || (page >= LCD9648_PAGE_COUNT) ||
        (integerDigits == 0U) || (integerDigits > 9U) ||
        (decimalPlaces > 6U) ||
        ((integerDigits + decimalPlaces) > 9U))
    {
        return;
    }

    for (index = 0U; index < decimalPlaces; index++)
    {
        scale *= 10UL;
    }

    fieldColumns = integerDigits * 6U;
    if (decimalPlaces > 0U)
    {
        fieldColumns += 3U + decimalPlaces * 6U;
    }

    clearColumns = fieldColumns;
    if (clearColumns > (LCD9648_WIDTH - x))
    {
        clearColumns = LCD9648_WIDTH - x;
    }

    LCD9648_SetCursor(x, page);
    for (column = 0U; column < clearColumns; column++)
    {
        LCD9648_WriteData(0x00U);
    }

    if ((fieldColumns > (LCD9648_WIDTH - x)) || (value < 0.0F) ||
        (value > (4294967040.0F / (float)scale)))
    {
        return;
    }

    scaledValue = (Uint32)(value * (float)scale + 0.5F);
    integerPart = scaledValue / scale;
    fractionPart = scaledValue % scale;

    if (integerPart == 0UL)
    {
        digits[0] = 0U;
        digitCount = 1U;
    }
    else
    {
        while ((integerPart > 0UL) && (digitCount < 10U))
        {
            digits[digitCount] = (Uint16)(integerPart % 10UL);
            integerPart /= 10UL;
            digitCount++;
        }
    }

    if (digitCount > integerDigits)
    {
        return;
    }

    drawX = x + (integerDigits - digitCount) * 6U;
    while (digitCount > 0U)
    {
        digitCount--;
        LCD9648_DrawDigit(drawX, page, digits[digitCount]);
        drawX += 6U;
    }

    if (decimalPlaces == 0U)
    {
        return;
    }

    drawX = x + integerDigits * 6U;
    LCD9648_DrawDecimalPoint(drawX, page);
    drawX += 3U;

    divisor = scale / 10UL;
    for (index = 0U; index < decimalPlaces; index++)
    {
        LCD9648_DrawDigit(drawX, page,
                         (Uint16)(fractionPart / divisor));
        fractionPart %= divisor;
        divisor /= 10UL;
        drawX += 6U;
    }
}
