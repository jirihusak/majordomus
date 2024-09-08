/* 
 * File:   lcd_NHDC12832.h
 * Author: jiri
 *
 * Created on 16. ledna 2018, 20:05
 */

#ifndef LCD_NHDC12832_H
#define	LCD_NHDC12832_H

#include <stdint.h>

enum {
    FONT_TAHOMA_16 = 0,
    FONT_TAHOMA_BOLD_16 = 1,
    FONT_TAHOMA_8 = 2,
    FONT_SYMBOLS_32 = 3,
    FONT_SYMBOLS_16 = 4,
    FONT_PACIFICO_16 = 5,
    FONT_UBUNTU_16 = 6,
    FONT_UBUNTU_BOLD_16 = 7,
    FONT_END = 8
};

#define LCD_LINE_SMALL_1    0
#define LCD_LINE_SMALL_2    1
#define LCD_LINE_SMALL_3    2
#define LCD_LINE_SMALL_4    3

#define LCD_LINE_BIG_1      0
#define LCD_LINE_BIG_2      1
#define LCD_LINE_BIG_3      2


void lcd_NHDC12832_Init();

void lcd_clear_display();
void lcd_draw_string(int font, uint8_t line, uint8_t col, char *c);
void lcd_draw_bitmap(int font, uint8_t line, uint8_t col, int index);

#endif	/* LCD_NHDC12832_H */

