/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>

/* HAL Includes */
#include "ButtonLED_HAL.h"
#include "Timer32_HAL.h"
#include "LCD_display_HAL.h"
// TODO: add the uart HAL include statement here  to include the files in building the project
#include "UART_HAL.h"

/* Other Includes */
//#include <stdbool.h>
#include <string.h>
extern HWTimer_t timer0, timer1;

// TODO: feel free to add
void init_all(Graphics_Context *g_sContext_p);

void switch_baud(eUSCI_UART_Config *uartConfig, UARTBaudRate_t *baudRate);

void edit_mode(eUSCI_UART_Config *uartConfig, Graphics_Context *g_sContext, uint8_t *string_size, int8_t *text_string,
               int32_t *x_pos, int32_t *y_pos, uint8_t* reset);
void format_mode(eUSCI_UART_Config *uartConfig, Graphics_Context *g_sContext, uint8_t *iter, uint8_t* c_char,
                 int8_t *text_string, int32_t *move_pos, int32_t *x_pos, int32_t *y_pos, uint8_t *font_size, uint8_t* font_type, uint8_t* reset);
void format_font(Graphics_Context *g_sContext, uint8_t *c_char, uint8_t *font_type, uint8_t *font_size);

int main(void)
{
    Graphics_Context g_sContext;

    init_all(&g_sContext);

    eUSCI_UART_Config uartConfig = {
    EUSCI_A_UART_CLOCKSOURCE_SMCLK,               // SMCLK Clock Source = 48MHz
            312,                                          // UCBR   = 312
            8,                                            // UCBRF  = 8
            0x00,                                         // UCBRS  = 0x00
            EUSCI_A_UART_NO_PARITY,                       // No Parity
            EUSCI_A_UART_LSB_FIRST,                       // LSB First
            EUSCI_A_UART_ONE_STOP_BIT,                    // One stop bit
            EUSCI_A_UART_MODE,                            // UART mode
            EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION // Oversampling
            };

    InitUART(EUSCI_A0_BASE, &uartConfig, GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3);

    // BoosterPack top button
    button_t B_S1;
    initButton(&B_S1, GPIO_PORT_P5, GPIO_PIN1, &timer0);

    // BoosterPack bottom button
    button_t B_S2;
    initButton(&B_S2, GPIO_PORT_P3, GPIO_PIN5, &timer0);

    UARTBaudRate_t baudRate = baud9600;

    bool mode = 0;
    uint8_t iter = 0;
    uint8_t c_char, font_type;
    uint8_t reset = 0;
    uint8_t font_size = 12;
    uint8_t string_size = 0;
    int8_t text_string[10] = {};
    int32_t move_pos = 0;
    int32_t x_pos = 10;
    int32_t y_pos = 20;

    while (1)
    {
        if (ButtonPushed(&B_S1))
            mode = !mode;

        if (ButtonPushed(&B_S2))
            switch_baud(&uartConfig, &baudRate);

    if (mode == 0)
        edit_mode(&uartConfig, &g_sContext, &string_size, text_string, &x_pos, &y_pos, &reset);
    else
        format_mode(&uartConfig, &g_sContext, &iter, &c_char, text_string, &move_pos, &x_pos, &y_pos, &font_size, &font_type, &reset);
    }
}

void format_mode(eUSCI_UART_Config *uartConfig, Graphics_Context *g_sContext, uint8_t *iter, uint8_t* c_char,
                 int8_t *text_string, int32_t *move_pos, int32_t *x_pos, int32_t *y_pos, uint8_t *font_size, uint8_t* font_type, uint8_t* reset)
{
    char r_char;
    if (UARTHasChar(EUSCI_A0_BASE)) {
        r_char = UARTGetChar(EUSCI_A0_BASE);
        if (UARTCanSend(EUSCI_A0_BASE))
            UARTPutChar(EUSCI_A0_BASE, r_char);
        // A valid command key has been entered and it hasn't already received a valid key
        if (*iter == 0 && (r_char == 'c' || r_char == 'b' || r_char == 'm' || r_char == 's' || r_char == 't' || r_char == 'r')) {
            *iter = 1;
            *c_char = r_char;
        }

        else if (*iter != 0 && (*c_char == 'c' || *c_char == 'b' || *c_char == 'm'|| *c_char == 's' || *c_char == 't' || *c_char == 'r')) {
            if (*c_char == 'c') {
                // font color settings
                if (r_char == 'l') {
                    Graphics_setForegroundColor(g_sContext, GRAPHICS_COLOR_BLACK);
                    Graphics_clearDisplay(g_sContext);
                    Graphics_drawString(g_sContext, text_string, -1, *x_pos, *y_pos, true);
                }
                else if (r_char == 'r') {
                    Graphics_setForegroundColor(g_sContext, GRAPHICS_COLOR_RED);
                    Graphics_clearDisplay(g_sContext);
                    Graphics_drawString(g_sContext, text_string, -1, *x_pos, *y_pos, true);
                }
                else if (r_char == 'g') {
                    Graphics_setForegroundColor(g_sContext, GRAPHICS_COLOR_GREEN);
                    Graphics_clearDisplay(g_sContext);
                    Graphics_drawString(g_sContext, text_string, -1, *x_pos, *y_pos, true);
                }
                else if (r_char == 'y') {
                    Graphics_setForegroundColor(g_sContext, GRAPHICS_COLOR_YELLOW);
                    Graphics_clearDisplay(g_sContext);
                    Graphics_drawString(g_sContext, text_string, -1, *x_pos, *y_pos, true);
                }
                else if (r_char == 'b') {
                    Graphics_setForegroundColor(g_sContext, GRAPHICS_COLOR_BLUE);
                    Graphics_clearDisplay(g_sContext);
                    Graphics_drawString(g_sContext, text_string, -1, *x_pos, *y_pos, true);
                }
                else if (r_char == 'm') {
                    Graphics_setForegroundColor(g_sContext, GRAPHICS_COLOR_MAGENTA);
                    Graphics_clearDisplay(g_sContext);
                    Graphics_drawString(g_sContext, text_string, -1, *x_pos, *y_pos, true);
                }
                else if (r_char == 'c') {
                    Graphics_setForegroundColor(g_sContext, GRAPHICS_COLOR_CYAN);
                    Graphics_clearDisplay(g_sContext);
                    Graphics_drawString(g_sContext, text_string, -1, *x_pos, *y_pos, true);
                }
                else if (r_char == 'w') {
                    Graphics_setForegroundColor(g_sContext, GRAPHICS_COLOR_WHITE);
                    Graphics_clearDisplay(g_sContext);
                    Graphics_drawString(g_sContext, text_string, -1, *x_pos, *y_pos, true);
                }
                *iter = 0; // command is done or invalid, reset and look for new command
            }
            else if (*c_char == 'b') {
                // background color
                if (r_char == 'l') {
                    Graphics_setBackgroundColor(g_sContext, GRAPHICS_COLOR_BLACK);
                    Graphics_clearDisplay(g_sContext);
                    Graphics_drawString(g_sContext, text_string, -1, *x_pos, *y_pos, true);
                }
                else if (r_char == 'r') {
                    Graphics_setBackgroundColor(g_sContext, GRAPHICS_COLOR_RED);
                    Graphics_clearDisplay(g_sContext);
                    Graphics_drawString(g_sContext, text_string, -1, *x_pos, *y_pos, true);
                }
                else if (r_char == 'g') {
                    Graphics_setBackgroundColor(g_sContext, GRAPHICS_COLOR_GREEN);
                    Graphics_clearDisplay(g_sContext);
                    Graphics_drawString(g_sContext, text_string, -1, *x_pos, *y_pos, true);
                }
                else if (r_char == 'y') {
                    Graphics_setBackgroundColor(g_sContext, GRAPHICS_COLOR_YELLOW);
                    Graphics_clearDisplay(g_sContext);
                    Graphics_drawString(g_sContext, text_string, -1, *x_pos, *y_pos, true);
                }
                else if (r_char == 'b') {
                    Graphics_setBackgroundColor(g_sContext, GRAPHICS_COLOR_BLUE);
                    Graphics_clearDisplay(g_sContext);
                    Graphics_drawString(g_sContext, text_string, -1, *x_pos, *y_pos, true);
                }
                else if (r_char == 'm') {
                    Graphics_setBackgroundColor(g_sContext, GRAPHICS_COLOR_MAGENTA);
                    Graphics_clearDisplay(g_sContext);
                    Graphics_drawString(g_sContext, text_string, -1, *x_pos, *y_pos, true);
                }
                else if (r_char == 'c') {
                    Graphics_setBackgroundColor(g_sContext, GRAPHICS_COLOR_CYAN);
                    Graphics_clearDisplay(g_sContext);
                    Graphics_drawString(g_sContext, text_string, -1, *x_pos, *y_pos, true);
                }
                else if (r_char == 'w') {
                    Graphics_setBackgroundColor(g_sContext, GRAPHICS_COLOR_WHITE);
                    Graphics_clearDisplay(g_sContext);
                    Graphics_drawString(g_sContext, text_string, -1, *x_pos, *y_pos, true);
                }
                *iter = 0; // command is done or invalid, reset and look for new command
            }
            else if (*c_char == 'm') {
                // movement
                if (*iter == 1) {
                    *move_pos = (int)((r_char - '0') * 10); // add digit in tens place
                    *iter = 2;
                }
                else if (*iter == 2) {
                    *move_pos = *move_pos + (int)(r_char - '0'); // add digit in ones place
                    *iter = 3;
                }
                else if (*iter == 3) {
                    if (r_char == 'u') {
                        *y_pos = *y_pos - *move_pos;

                    }
                    else if (r_char == 'd') {
                        *y_pos = *y_pos + *move_pos;

                    }
                    else if (r_char == 'l') {
                        *x_pos = *x_pos - *move_pos;

                    }
                    else if (r_char == 'r') {
                        *x_pos = *x_pos + *move_pos;
                    }
                    Graphics_clearDisplay(g_sContext);
                    Graphics_drawString(g_sContext, text_string, -1, *x_pos, *y_pos, true);
                    *iter = 0; // command run, reset
                }
            }
            else if (*c_char == 's' || *c_char == 't') {
                // font size and type
                if (*c_char == 's') { // update font size
                    if (r_char == 'b')
                        *font_size = (*font_size < 22) ? *font_size + 2 : *font_size; // increase font_size by 2 if not at max (22)
                    else if (r_char == 's')
                        *font_size = (*font_size > 12) ? *font_size - 2 : *font_size; // decrease fon_size by 2 if not at min (12)
                }
                else
                    *font_type = r_char; // update font type
                format_font(g_sContext, c_char, font_type, font_size);
                *iter = 0;
            }
            else {
                *reset = 1; // set value to be used in edit mode
                *iter = 0;
            }

        }
    }
}

void format_font(Graphics_Context *g_sContext, uint8_t *c_char, uint8_t *font_type, uint8_t *font_size) {
    if (*font_type == 'c' && *font_size == 12) {
        Graphics_setFont(g_sContext, &g_sFontCm12);
    }
    else if (*font_type == 'c' && *font_size == 14) {
        Graphics_setFont(g_sContext, &g_sFontCm14);
    }
    else if (*font_type == 'c' && *font_size == 16) {
        Graphics_setFont(g_sContext, &g_sFontCm16);
    }
    else if (*font_type == 'c' && *font_size == 18) {
        Graphics_setFont(g_sContext, &g_sFontCm18);
    }
    else if (*font_type == 'c' && *font_size == 20) {
        Graphics_setFont(g_sContext, &g_sFontCm20);
    }
    else if (*font_type == 'c' && *font_size == 22) {
        Graphics_setFont(g_sContext, &g_sFontCm22);
    }
    else if (*font_type == 's' && *font_size == 12) {
        Graphics_setFont(g_sContext, &g_sFontCmss12);
    }
    else if (*font_type == 's' && *font_size == 14) {
        Graphics_setFont(g_sContext, &g_sFontCmss14);
    }
    else if (*font_type == 's' && *font_size == 16) {
        Graphics_setFont(g_sContext, &g_sFontCmss16);
    }
    else if (*font_type == 's' && *font_size == 18) {
        Graphics_setFont(g_sContext, &g_sFontCmss18);
    }
    else if (*font_type == 's' && *font_size == 20) {
        Graphics_setFont(g_sContext, &g_sFontCmss20);
    }
    else if (*font_type == 's' && *font_size == 22) {
        Graphics_setFont(g_sContext, &g_sFontCmss22);
    }
    else if (*font_type == 't' && *font_size == 12) {
        Graphics_setFont(g_sContext, &g_sFontCmtt12);
    }
    else if (*font_type == 't' && *font_size == 14) {
        Graphics_setFont(g_sContext, &g_sFontCmtt14);
    }
    else if (*font_type == 't' && *font_size == 16) {
        Graphics_setFont(g_sContext, &g_sFontCmtt16);
    }
    else if (*font_type == 't' && *font_size == 18) {
        Graphics_setFont(g_sContext, &g_sFontCmtt18);
    }
    else if (*font_type == 't' && *font_size == 20) {
        Graphics_setFont(g_sContext, &g_sFontCmtt20);
    }
    else if (*font_type == 't' && *font_size == 22) {
        Graphics_setFont(g_sContext, &g_sFontCmtt22);
    }
}

void edit_mode(eUSCI_UART_Config *uartConfig, Graphics_Context *g_sContext, uint8_t *string_size, int8_t *text_string, int32_t *x_pos, int32_t *y_pos, uint8_t* reset)
{
    char r_char;
    int32_t cursor_x;
    //int32_t cursor_y = *y_pos;

    if (UARTHasChar(EUSCI_A0_BASE))
    {
        r_char = UARTGetChar(EUSCI_A0_BASE);

        if (r_char == 127) {
            if (*string_size > 0) {
                text_string[*string_size - 1] = '\0';
                *string_size = *string_size - 1;
            }
            cursor_x = *x_pos + Graphics_getStringWidth(g_sContext, text_string, -1);
        }

        else if (*string_size < 10)
        {
            text_string[*string_size] = r_char;
            cursor_x = *x_pos + Graphics_getStringWidth(g_sContext, text_string, -1);
            if (*string_size == 9)
                cursor_x = *x_pos;
            ++*string_size;
        }
        else
        {
            memset(text_string, '\0', 10);
            text_string[0] = r_char;
            if (*reset == 1) {
                Graphics_setFont(g_sContext, &g_sFontCmss12);
                Graphics_setBackgroundColor(g_sContext, GRAPHICS_COLOR_BLACK);
                Graphics_setForegroundColor(g_sContext, GRAPHICS_COLOR_WHITE);
                *reset = 0;
            }
            cursor_x = *x_pos + Graphics_getStringWidth(g_sContext, text_string, -1);
            *string_size = 1;
        }
        Graphics_clearDisplay(g_sContext);
        Graphics_drawString(g_sContext, text_string, -1, *x_pos, *y_pos, true);
        Graphics_drawLineV(g_sContext, cursor_x, *y_pos, *y_pos + Graphics_getFontHeight(g_sContext->font));

        if (UARTCanSend(EUSCI_A0_BASE))
            UARTPutChar(EUSCI_A0_BASE, r_char);
    }
}

void switch_baud(eUSCI_UART_Config *uartConfig, UARTBaudRate_t *baudRate)
{
switch (*baudRate)
{
case baud9600:
    turnOff_BoosterpackLED_red();
    turnOn_BoosterpackLED_green();
    *baudRate = baud19200;
    break;

case baud19200:
    turnOff_BoosterpackLED_green();
    turnOn_BoosterpackLED_blue();
    *baudRate = baud38400;
    break;

case baud38400:
    turnOn_BoosterpackLED_red();
    turnOn_BoosterpackLED_green();
    *baudRate = baud57600;
    break;

case baud57600:
    turnOff_BoosterpackLED_green();
    turnOff_BoosterpackLED_blue();
    *baudRate = baud9600;
    break;
}
UARTSetBaud(EUSCI_A0_BASE, uartConfig, *baudRate);
}

void init_leds()
{
initialize_LaunchpadLED1();
initialize_LaunchpadLED2_red();
initialize_BoosterpackLED_red();
initialize_BoosterpackLED_green();
initialize_BoosterpackLED_blue();

turnOff_LaunchpadLED1();
turnOff_LaunchpadLED2_red();
turnOn_BoosterpackLED_red();
turnOff_BoosterpackLED_green();
turnOff_BoosterpackLED_blue();

}

void init_buttons()
{
initialize_LaunchpadRightButton();
initialize_LaunchpadLeftButton();

initialize_BoosterpackTopButton();
initialize_BoosterpackBottomButton();
}

void init_timers()
{
initHWTimer0();
initHWTimer1();
}

void init_all(Graphics_Context *g_sContext_p)
{
WDT_A_hold(WDT_A_BASE);

init_leds();
init_buttons();
init_timers();
init_graphics(g_sContext_p);

// TODO: add more initialization function calls here
}
