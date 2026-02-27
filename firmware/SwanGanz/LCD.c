#include <ti/devices/msp/msp.h>
#include "../inc/ST7735.h"


static void LCD_Start() {
    ST7735_OutString("test");
}
void LCD_Init() {
    ST7735_InitR(INITR_BLACKTAB); //INITR_REDTAB for AdaFruit
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_SetCursor(0, 0);
    LCD_Start();
}