#include <stdio.h>
#include <stdbool.h>
#include "drivers/lcd_NHDC12832.h"
#include "drivers/tick.h"
#include "gui.h"
#include "mcc_generated_files/pin_manager.h"
#include "app.h"
#include "mcc_generated_files/pwm5.h"
#include "device_memory.h"
#include "drivers/piezo.h"



enum display_page {
    PAGE_HOME,
    PAGE_PAGE_TEMP_SETTING,
    PAGE_PAGE_TEMP_EDIT,
    PAGE_VIEW_SENSORS,    
    
    PAGE_DOOR_BELL,
};


static int actual_page = PAGE_HOME;
static bool screen_active = false;
static uint32_t screen_saver_time = 0;
static uint32_t update_display_timer = 0;
static bool update_display = false;
static uint8_t lcd_back_light_request;
static uint8_t lcd_back_light_actual;
static uint32_t update_time = 1000;
static uint32_t edit_timeout;
//static uint8_t update_bank = GLCD_NUMBER_OF_BANKS;

void lcd_show_home()
{

    char line1[20];
//    sprintf(line1, "%d %d %d ", meas_values.temperature, meas_values.humidity, meas_values.voc);
//    lcd_draw_string(FONT_TAHOMA_16, LCD_LINE_BIG_1, 0, line1);
//
//    
//    sprintf(line1, "%d %d %lu ", meas_values.motion, meas_values.noise, meas_values.lux);
//    lcd_draw_string(FONT_TAHOMA_16, LCD_LINE_BIG_3, 0, line1);
    
    // temperature
    lcd_draw_bitmap(FONT_SYMBOLS_16, LCD_LINE_BIG_1, 13, 0);
    if(meas_values.temperature != INT16_MIN)
        sprintf(line1, "%d.%d° ", meas_values.temperature / 10, meas_values.temperature % 10);
    else
        sprintf(line1, " --");
    lcd_draw_string(FONT_UBUNTU_16, LCD_LINE_BIG_3, 5, line1);
    
    // humididty
    lcd_draw_bitmap(FONT_SYMBOLS_16, LCD_LINE_BIG_1, 63, 1);
    if(meas_values.humidity != INT16_MIN)
        sprintf(line1, "%d%% ", meas_values.humidity / 10);
    else 
        sprintf(line1, " --");
    lcd_draw_string(FONT_UBUNTU_16, LCD_LINE_BIG_3, 50, line1);
       
    // air quality
    lcd_draw_bitmap(FONT_SYMBOLS_16, LCD_LINE_BIG_1, 88+8, 2);
    if(meas_values.voc != INT16_MIN)
        sprintf(line1, "%d ", meas_values.voc);
    else
        sprintf(line1, "--");
    lcd_draw_string(FONT_UBUNTU_16, LCD_LINE_BIG_3, 95, line1);
    
    update_display = false;
    update_time = 1000;
}

void lcd_show_temp_setting()
{
    //glcd_clear_buffer();

    lcd_draw_bitmap(FONT_SYMBOLS_32, LCD_LINE_BIG_1, 5, 0);
    
    char line1[20];
    sprintf(line1, "%d.%d°C ", control_struct.req_temp / 10, control_struct.req_temp % 10);
    lcd_draw_string(FONT_UBUNTU_BOLD_16, LCD_LINE_BIG_2, 38, line1);
    //glcd_write();
    
    update_display = false;
    update_time = 1000;
}

void lcd_show_temp_edit()
{
    static int8_t edit_text_toggle = 1;
    edit_text_toggle ^= 1;

    lcd_draw_bitmap(FONT_SYMBOLS_32, LCD_LINE_BIG_1, 5, 0);
    
    char line1[20];
    if(edit_text_toggle)
        sprintf(line1, "%d.%d°C", control_struct.new_req_temp / 10, control_struct.new_req_temp % 10);
    else 
        sprintf(line1, "       ");
    
    lcd_draw_string(FONT_UBUNTU_BOLD_16, LCD_LINE_BIG_2, 38, line1);
    //glcd_write();
    
    update_display = false;
    update_time = 250;
}


void lcd_show_view_sensors()
{

    char line1[20];
    
    // motion
    lcd_draw_bitmap(FONT_SYMBOLS_16, LCD_LINE_BIG_1, 10, 5);
    sprintf(line1, "%d  ", meas_values.noise/10);
    lcd_draw_string(FONT_UBUNTU_16, LCD_LINE_BIG_3, 8, line1);
    
    // noise
    lcd_draw_bitmap(FONT_SYMBOLS_16, LCD_LINE_BIG_1, 50, 3);
    sprintf(line1, "%d ", meas_values.motion );
    lcd_draw_string(FONT_UBUNTU_16, LCD_LINE_BIG_3, 55, line1);
       
    // lux
    lcd_draw_bitmap(FONT_SYMBOLS_16, LCD_LINE_BIG_1, 92, 4);
    if(meas_values.lux != INT16_MIN)
        sprintf(line1, "%4lu   ", meas_values.lux);
    else
        sprintf(line1, "--");
    lcd_draw_string(FONT_UBUNTU_16, LCD_LINE_BIG_3, 80, line1);
    
    update_display = false;
    update_time = 1000;
    
}

///////////////////////////////////////////////////////////////////////////
void gui_Init()
{
    //LCD_LED_PWR_SetLow();
    lcd_back_light_actual = 50;
    lcd_back_light_request = 50;
    PWM5_LoadDutyValue(lcd_back_light_actual*10);
    
    lcd_NHDC12832_Init();
        
}

void gui_Task(button_control_t *button_control)
{
    
    if( button_control->enterRisingEdge || button_control->minusRisingEdge || button_control->plusRisingEdge)
    {
        // first touch
        if(screen_active == false) 
        {
            button_control->enterRisingEdge = false;
            button_control->minusRisingEdge = false;
            button_control->plusRisingEdge = false;
        }
        
        screen_active = true;
        screen_saver_time = tick_getTick();
        lcd_back_light_request = (meas_values.lux < 50) ? 30 : 100;
    }
    
    if(tick_getTick() - screen_saver_time > (60*TICK_SECOND))
    {
        screen_active = false;
        lcd_back_light_request = (meas_values.lux < 50) ? 2 : 12;
    }
    
    // timer for update display
    if(tick_getTick() - update_display_timer > (update_time*TICK_MILISECOND))
    {
        update_display = true;
        update_display_timer = tick_getTick();
    }
    
    switch(actual_page)
    {
        case PAGE_HOME:
            if(update_display)
                lcd_show_home();
            
            if(button_control->minusRisingEdge) {
                actual_page = PAGE_VIEW_SENSORS;
                lcd_clear_display();
                update_display = true;
            }
            
            if(button_control->plusRisingEdge) {
                actual_page = PAGE_PAGE_TEMP_SETTING;
                lcd_clear_display();
                update_display = true;
            }
            break;
            
        case PAGE_PAGE_TEMP_SETTING:
            if(update_display)
                lcd_show_temp_setting();
            
            if(button_control->minusRisingEdge) {
                actual_page = PAGE_HOME;
                lcd_clear_display();
                update_display = true;
            }
            
            if(button_control->plusRisingEdge) {
                actual_page = PAGE_VIEW_SENSORS;
                lcd_clear_display();
                update_display = true;
            }
            
            if(button_control->enterRisingEdge) {
                actual_page = PAGE_PAGE_TEMP_EDIT;
                edit_timeout = tick_getTick();
                control_struct.new_req_temp = control_struct.req_temp;
                lcd_clear_display();
                update_display = true;
            }
            break;
            
        case PAGE_PAGE_TEMP_EDIT:
            if(update_display)
                lcd_show_temp_edit();
            
            if(button_control->minusRisingEdge) {
                edit_timeout = tick_getTick();
                if(control_struct.new_req_temp > 10) 
                    control_struct.new_req_temp -= 1;
            }
            else if(button_control->minusPressHold) {
                edit_timeout = tick_getTick();
                if(control_struct.new_req_temp > 11) 
                    control_struct.new_req_temp = (control_struct.new_req_temp - (control_struct.new_req_temp %5)) - 5;
                if(control_struct.new_req_temp < 10)
                    control_struct.new_req_temp = 10;
            }
            
            if(button_control->plusRisingEdge) {
                edit_timeout = tick_getTick();
                if(control_struct.new_req_temp < 300) 
                    control_struct.new_req_temp += 1;
            }
            else if(button_control->plusPressHold) {
                edit_timeout = tick_getTick();
                if(control_struct.new_req_temp < 300) 
                    control_struct.new_req_temp = (control_struct.new_req_temp - (control_struct.new_req_temp %5)) + 5;
                if(control_struct.new_req_temp > 300)
                    control_struct.new_req_temp = 300;
            }
            
            if((tick_getTick() - edit_timeout) > 5000)
            {
                actual_page = PAGE_PAGE_TEMP_SETTING;
                lcd_clear_display();
                update_display = true;
            }
            
            if(button_control->enterRisingEdge) {
                actual_page = PAGE_PAGE_TEMP_SETTING;
                control_struct.req_temp_trigger = true;
                //lcd_clear_display();
                piezo_beep_start(SHORT_BEEP);
                update_time = 1500;
                // aby tam neproblikla stara hodnota, nez se to zesynchronizuje, tak to odlozi update dipleje o 1sec
                update_display_timer = tick_getTick();
                //update_display = true;
            }
            break;
            
        case PAGE_VIEW_SENSORS:
            if(update_display)
                lcd_show_view_sensors();
            
            if(button_control->minusRisingEdge) {
                actual_page = PAGE_PAGE_TEMP_SETTING;
                lcd_clear_display();
                update_display = true;
            }
            
            if(button_control->plusRisingEdge) {
                actual_page = PAGE_HOME;
                lcd_clear_display();
                update_display = true;
            }
            break;
                
    }
    
    button_control->enterRisingEdge = false;
    button_control->minusRisingEdge = false;
    button_control->plusRisingEdge = false;
    
    button_control->enterPressHold = false;
    button_control->minusPressHold = false;
    button_control->plusPressHold = false;
            
   
    if(lcd_back_light_request > lcd_back_light_actual)
    {
        lcd_back_light_actual += 1;
    }
    if(lcd_back_light_request < lcd_back_light_actual)
    {
        lcd_back_light_actual -= 1;
    }

    PWM5_LoadDutyValue(lcd_back_light_actual*10);

}

