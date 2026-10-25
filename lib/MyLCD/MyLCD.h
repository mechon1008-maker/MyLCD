/*

- HD44780 LCD Library interface (learning version)
- Sources: HD44870.pdf, 16x2 LCD datsheet.pdf

*/

#ifndef MY_LCD_H
#define MY_LCD_H

#include <stdint.h>

// HAL CONTRACT: WHAT USER MUST PROVIDE
typedef struct LCD_HAL_t {
    void (*pinMode)(uint8_t pin, uint8_t mode);
    void (*digitalWrite)(uint8_t pin, uint8_t level);
    void (*delayUs)(uint32_t us);
    void (*delayMs)(uint32_t ms);
}LCD_HAL_t;

// COMMANDS FROM HD44780 AND 16X2 LCD LIBRARY INSTRUCTION SET
#define LCD_CLEARDISPLAY   0x01 // TO CLEAR DISPLAY
#define LCD_RETURNHOME     0x02 // RETURN HOME
#define LCD_ENTRYMODESET   0x04 // ENTRY MODE SET (INCREMENT CURSOR BY 1, SHIFT BY 0)
#define LCD_DISPLAYCONTROL 0x08 // DISPLAY CONTROL
#define LCD_FUNCTIONSET    0x20 // SET INTERFACE MODE

// Mask Control FUNCTIONSET
#define LCD_4BITMODE  0x00 //DL=0
#define LCD_ONELINE   0x00 //N=0
#define LCD_TWOLINE   0x08 //N=1
#define LCD_5x8DOTS   0x00 //F=0(5X8)

// Mask Control display
#define LCD_DISPLAYON  0x04 // display ON
#define LCD_CURSORON   0x02 // Cursor ON
#define LCD_BLINKON    0x01 // BLINK ON

// Mask Entry Mode SET
#define LCD_INCREMENT  0x02 // increment by 1
#define LCD_SHIFT      0x01 // Shift display

// HAL LEVEL CONSTANTS (values passed through the HAL function pointers)
#define LCD_INPUT      0
#define LCD_OUTPUT     1
#define LCD_LOW        0
#define LCD_HIGH       1

// Set DDRAM & CGRAM ADDRESS
#define LCD_SETDDRAMADDR 0x80 // DDRAM ADDRESS
#define LCD_SETCGRAMADDR 0x40 // CGRAM ADDRESS


// CLASS DEFINITION
class MyLCD{
public:
    // Constructor: USER PROVIDES HAL and PIN NUMBERS
    MyLCD(LCD_HAL_t* hal, uint8_t rs, uint8_t enable,
         uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);

    // INITIALIZATION: MUST CALL BEFORE USING LCD
    void begin(uint8_t cols, uint8_t rows);

    // BASIC COMMANDS
    void clear();   // clear display
    void setCursor(uint8_t col, uint8_t row); // set cursor position: col = column (x), row = line (y)
    void writeChar(uint8_t c); // writes one character on screen
    void displayOn();
    void displayOff();
    void cursorOn();
    void cursorOff();
    void blinkOn();
    void blinkOff();
    void createChar(uint8_t slot, const uint8_t *pattern);
    void home();
    void print(const char* str);

private:
    // PRIVATE HELPERS: INTERNAL IMPLEMENTATION
    void sendCommand(uint8_t cmd);
    void sendData(uint8_t data);
    void write4bits(uint8_t value);
    void send(uint8_t value, uint8_t mode);
    void pulseEnable();

    // PRIVATE STATE: WHAT WE NEED TO REMEMBER
    LCD_HAL_t* _hal = nullptr;        // Pointer to HAL
    uint8_t _rs = 0;            // Store register select pin
    uint8_t _en = 0;            // Store enable pin
    uint8_t _data_pins[4] = {0,0,0,0};  // Store data_pins DB7-DB4
    uint8_t _cols = 0;
    uint8_t _rows = 0;
    uint8_t _displayControl = LCD_DISPLAYON; // control display state
};
#endif //MY_LCD_H