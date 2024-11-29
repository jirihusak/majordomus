
#include "stdint.h"
#include "../mcc_generated_files/mcc.h"
#include <xc.h>
#include "lcd_NHDC12832.h"

#include "../fonts/Tahoma8px.h"
#include "../fonts/Tahoma16px.h"
#include "../fonts/TahomaBold16px.h"
#include "../fonts/Symbols32px.h"
#include "../fonts/Symbols16px.h"
#include "../fonts/Pacifico16px.h"
#include "../fonts/Ubuntu16px.h"
#include "../fonts/UbuntuBold16px.h"

// Pins
#define GLCD_SELECT()     LCD_CS_SetLow()
#define GLCD_DESELECT()   LCD_CS_SetHigh()
#define GLCD_DC_LOW()     NOP()
#define GLCD_DC_HIGH()    NOP()
#define GLCD_RESET_LOW()  NOP()//LCD_RES_SetLow()
#define GLCD_RESET_HIGH() NOP()//LCD_RES_SetHigh()
#define GLCD_A0_LOW()     LCD_A0_SetLow() /* Command write */
#define GLCD_A0_HIGH()    LCD_A0_SetHigh() /* Write to display RAM */

// Commands
#define ST7565R_DISPLAY_ON               0xAF /* 0b10101111 */
#define ST7565R_DISPLAY_OFF              0xAE /* 0b10101110 */
#define ST7565R_PAGE_ADDRESS_SET         0xB0 /* 0b10110000 */
#define ST7565R_COLUMN_ADDRESS_SET_LOWER 0x00 
#define ST7565R_COLUMN_ADDRESS_SET_UPPER 0x10 
#define ST7565R_DISPLAY_NORMAL           0xA4 /* 0b10100100 */
#define ST7565R_DISPLAY_ALL_ON           0xA5 /* 0b10100101 */
#define ST7565R_NORMAL                   0xA0 /* 0b10100000 */
#define ST7565R_REVERSE                  0xA1 /* 0b10100001 */
#define ST7565R_RESET                    0xE2 /* 0b11100010 */

#define GLCD_NUMBER_OF_COLS              128  
#define GLCD_NUMBER_OF_ROWS              32   
#define GLCD_NUMBER_OF_BANKS             GLCD_NUMBER_OF_ROWS/8

typedef struct {
    unsigned char *char_table;
    int start_char;
    int end_char;
    int max_width;
    int max_height_lines;
}font_t;

font_t font_table[FONT_END];


// Private functions
void glcd_set_column_upper(uint8_t addr);
void glcd_set_column_lower(uint8_t addr);
void glcd_all_on(void);
void glcd_normal(void);
void glcd_set_start_line(uint8_t addr);
void lcd_clear_now(void);
void glcd_pattern(void);
void glcd_ST7565R_init(void);
void glcd_write_bank(uint8_t bank);
void glcd_command(uint8_t c);
void glcd_data(uint8_t c);
void glcd_reset(void);
void glcd_set_contrast(uint8_t val);
void glcd_spi_write(uint8_t c);

void lcd_NHDC12832_Init()
{
    // init fonts
    font_table[FONT_TAHOMA_16].char_table = Tahoma16;
    font_table[FONT_TAHOMA_16].start_char = 0x20;
    font_table[FONT_TAHOMA_16].end_char = 127;
    font_table[FONT_TAHOMA_16].max_width = 15;
    font_table[FONT_TAHOMA_16].max_height_lines = 2;
    
    
    font_table[FONT_TAHOMA_BOLD_16].char_table = TahomaBold16;
    font_table[FONT_TAHOMA_BOLD_16].start_char = 0x20;
    font_table[FONT_TAHOMA_BOLD_16].end_char = 128;
    font_table[FONT_TAHOMA_BOLD_16].max_width = 18;
    font_table[FONT_TAHOMA_BOLD_16].max_height_lines = 2;
    
    font_table[FONT_TAHOMA_8].char_table = Tahoma8;
    font_table[FONT_TAHOMA_8].start_char = 0x20;
    font_table[FONT_TAHOMA_8].end_char = 127;
    font_table[FONT_TAHOMA_8].max_width = 10;
    font_table[FONT_TAHOMA_8].max_height_lines = 1;
    
    // init fonts
    font_table[FONT_SYMBOLS_32].char_table = Symbols32px;
    font_table[FONT_SYMBOLS_32].start_char = 0;
    font_table[FONT_SYMBOLS_32].end_char = 32;
    font_table[FONT_SYMBOLS_32].max_width = 32;
    font_table[FONT_SYMBOLS_32].max_height_lines = 4;
    
    font_table[FONT_SYMBOLS_16].char_table = Symbols16px;
    font_table[FONT_SYMBOLS_16].start_char = 0;
    font_table[FONT_SYMBOLS_16].end_char = 32;
    font_table[FONT_SYMBOLS_16].max_width = 16;
    font_table[FONT_SYMBOLS_16].max_height_lines = 2;
    
    font_table[FONT_PACIFICO_16].char_table = Pacifico16;
    font_table[FONT_PACIFICO_16].start_char = 45;
    font_table[FONT_PACIFICO_16].end_char = 58;
    font_table[FONT_PACIFICO_16].max_width = 12;
    font_table[FONT_PACIFICO_16].max_height_lines = 2;
    
    font_table[FONT_UBUNTU_16].char_table = Ubuntu16;
    font_table[FONT_UBUNTU_16].start_char = 32;
    font_table[FONT_UBUNTU_16].end_char = 180;
    font_table[FONT_UBUNTU_16].max_width = 16;
    font_table[FONT_UBUNTU_16].max_height_lines = 2;
    
    font_table[FONT_UBUNTU_BOLD_16].char_table = UbuntuBold16;
    font_table[FONT_UBUNTU_BOLD_16].start_char = 32;
    font_table[FONT_UBUNTU_BOLD_16].end_char = 255;
    font_table[FONT_UBUNTU_BOLD_16].max_width = 16;
    font_table[FONT_UBUNTU_BOLD_16].max_height_lines = 2;
        
	/* Deselect LCD */
	GLCD_DESELECT();

	/* Send reset pulse to LCD */
    __delay_ms(5);
	glcd_reset();
	__delay_ms(10);

	/* Begin sending data for initialisation sequence */
	glcd_command(0xa0); /* ADC select in normal mode */
	glcd_command(0xae); /* Display OFF */
    glcd_command(0xad);	// Static indicator ON
	glcd_command(0x00);	// Static indicator register, not Blinking
	glcd_command(0xc8); /* Common output mode select: reverse direction (last 3 bits are ignored) */
	glcd_command(0xa2); /* LCD bias set at 1/9 */
	glcd_command(0x2f); /* Power control set to operating mode: 7 */
	glcd_command(0x21); /* Internal resistor ratio, set to: 1 */
	glcd_set_contrast(35); /* Set contrast, value experimentally determined, can set to 6-bit value, 0 to 63 */
    glcd_command(0xa6); /* normal mode - not inverted */
    glcd_command(0xa4); /* normal displej - not all point on*/
    glcd_command(0x40); /* offset to 0 */
    glcd_command(0xb0); /* page addr 0*/
    glcd_command(0x17); /* column addr 128px MSB*/
    glcd_command(0x0F); /* column addr 128px LSB*/
	glcd_command(0xaf); /* Display on */    
	
	lcd_clear_now();

}


// LOW LEVEL FUNCTIONS
void glcd_command(uint8_t c)
{
	GLCD_A0_LOW();
	glcd_spi_write(c);	
}

void glcd_data(uint8_t c)
{
	GLCD_A0_HIGH();
	glcd_spi_write(c);	
}

void glcd_set_contrast(uint8_t val) {
	/* Can set a 6-bit value (0 to 63)  */

	/* Must send this command byte before setting the contrast */
	glcd_command(0x81);
	
	/* Set the contrat value ("electronic volumne register") */
	if (val > 63) {
		glcd_command(63);
	} else {
		glcd_command(val);
	}
	return;
}

void glcd_power_down(void)
{
	/* Command sequence as in ST7565 datasheet */
	glcd_command(0xac);	// Static indicator off
	glcd_command(0x00);	// Static indicator register, not blinking
	glcd_command(0xae); // Display OFF
	glcd_command(0xa5); // Display all points ON

	/* Display is now in sleep mode */
}

void glcd_power_up(void)
{
	glcd_command(0xa4); // Display all points OFF
	glcd_command(0xad);	// Static indicator ON
	glcd_command(0x00);	// Static indicator register, not Blinking
	glcd_command(0xaf);

	return;
}

void glcd_set_y_address(uint8_t y)
{
	glcd_command(ST7565R_PAGE_ADDRESS_SET | (0x0F & y)); /* 0x0F = 0b00001111 */
}

void glcd_set_x_address(uint8_t x)
{
	glcd_set_column_upper(x);
	glcd_set_column_lower(x);	
}

void glcd_all_on(void)
{
	glcd_command(ST7565R_DISPLAY_ALL_ON);
}

void glcd_normal(void)
{
	glcd_command(ST7565R_DISPLAY_NORMAL);
}

void glcd_set_column_upper(uint8_t addr)
{
	glcd_command(ST7565R_COLUMN_ADDRESS_SET_UPPER | (addr >> 4));
}

void glcd_set_column_lower(uint8_t addr)
{
	glcd_command(ST7565R_COLUMN_ADDRESS_SET_LOWER | (0x0f & addr));
}

/** Clear the display immediately, does not buffer */
void lcd_clear_now(void)
{
	uint8_t page;
	for (page = 0; page < GLCD_NUMBER_OF_BANKS; page++) {
		uint8_t col;
		glcd_set_y_address(page);
		glcd_set_x_address(0);
		for (col = 0; col < GLCD_NUMBER_OF_COLS; col++) {
			glcd_data(0);
		}			
	}
}

void glcd_spi_write(uint8_t c)
{
	GLCD_SELECT();
    
    SPI1_Open(SPI1_DEFAULT);
    
    SPI1_ExchangeByte(c);
    
    SPI1_Close();
    
    GLCD_DESELECT();
}

void glcd_reset(void)
{
	/* Toggle RST low to reset. Minimum pulse 100ns on datasheet. */
   //GLCD_SELECT();
//	GLCD_RESET_LOW();
//	__delay_ms(10);
//	GLCD_RESET_HIGH();
//	GLCD_DESELECT();	
    
    // SW reset
    glcd_command(ST7565R_RESET);
}

////////////////////////////////////////////////////////////////////
void lcd_clear_display()
{
    lcd_clear_now();
}

uint8_t lcd_draw_char(int font, uint8_t line, uint8_t col, char c)
{
    
	if (c < font_table[font].start_char || c > font_table[font].end_char) {
		c = '.';
	}
    
    uint16_t char_index = (c - font_table[font].start_char) * ((font_table[font].max_width * font_table[font].max_height_lines) + 1);
    uint8_t char_width = font_table[font].char_table[char_index];
        
	for (uint8_t line_cnt = 0; line_cnt < font_table[font].max_height_lines; line_cnt++)
    {
        // set write adress
        glcd_set_y_address(line_cnt + line);
        glcd_set_x_address(col);
        
        for (uint8_t col_cnt = 0; col_cnt < (char_width); col_cnt++)
        {
            glcd_data( font_table[font].char_table[((c - font_table[font].start_char) * ((font_table[font].max_width * font_table[font].max_height_lines) + 1)) + (col_cnt * font_table[font].max_height_lines) + line_cnt + 1]);      
        }
        glcd_data(0); // blank 1 column
        
    }
       
    return char_width + 1;
    
}

void lcd_draw_string(int font, uint8_t line, uint8_t col, char *c)
{
	uint8_t width;

	if (col > (GLCD_NUMBER_OF_COLS - font_table[font].max_width)) {
		/* Character won't fit */
		return;
	}

	while (*c) {
		width = lcd_draw_char(font, line, col, *c);
		col += width;
		c++;
	}		
}

void lcd_draw_bitmap(int font, uint8_t line, uint8_t col, int index)
{
	if (index < font_table[font].start_char || index > font_table[font].end_char) {
		index = 0;
	}
    
    uint16_t symbol_index = (index - font_table[font].start_char) * ((font_table[font].max_width * font_table[font].max_height_lines) + 1);
    uint8_t symbol_width = font_table[font].char_table[symbol_index];
        
	for (uint8_t line_cnt = 0; line_cnt < font_table[font].max_height_lines; line_cnt++)
    {
        // set write adress
        glcd_set_y_address(line_cnt + line);
        glcd_set_x_address(col);
        
        for (uint8_t col_cnt = 0; col_cnt < (symbol_width); col_cnt++)
        {
            //glcd_data( font_table[font].char_table[((index - font_table[font].start_char) * ((font_table[font].max_width * font_table[font].max_height_lines) + 1)) + (col_cnt * font_table[font].max_height_lines) + line_cnt + 1]);      
            glcd_data( font_table[font].char_table[((index - font_table[font].start_char) * ((font_table[font].max_width * font_table[font].max_height_lines) + 1)) + (col_cnt + (font_table[font].max_width) * line_cnt) + 1]);      
        }
        glcd_data(0); // blank 1 column
        
    }
       
    //return symbol_width + 1;
		
}
