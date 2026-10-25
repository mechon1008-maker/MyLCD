#include "MyLCD.h"  // Header file inclusion

/* 

CONCPET 1: Scope Resolution Operator (::)
The "MyLCD::" tells the compiler: This function belongs to the MyLCD class.

Without it, the compiler would think these are standalone global function.

*/

// 2. Constructor
// We simply store the provided HAL pointer and pin numbers to private variables

MyLCD::MyLCD(
    LCD_HAL_t* hal, uint8_t rs, uint8_t enable,uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7):
    _hal(hal),_rs(rs),_en(enable),_data_pins{d4,d5,d6,d7}{}


// 3. BEGIN() -  THE INITIALIZATION SEQUENCE.

void MyLCD::begin(uint8_t cols, uint8_t rows)
{
    if (!_hal) return; // Guard: the user must provide a HAL
    _cols = cols;
    _rows = rows;
    // Step A: Configure all pin as output from the DEV BOARD.
    _hal->pinMode(_rs, LCD_OUTPUT); // Setting register_select pin as output.
    _hal->pinMode(_en, LCD_OUTPUT); // Setting enable pin as output 
    for(int i = 0; i<4; i++) // Setting data_pins as output 
        _hal->pinMode(_data_pins[i], LCD_OUTPUT);
    
    _hal->digitalWrite(_rs, LCD_LOW);
    // Step B: The 4-bit Initialization sequence (from HD44780 Datasheet) 
    // Wait for atleast 50ms after power on
    _hal->delayMs(50);

    // 1st time send 0x03 function_set (8 bit mode) - Only HIGH NIBBLE
    write4bits(0x03);
    _hal->delayMs(10); // wait for > 4.1ms

    // 2nd time
    write4bits(0x03);
    _hal->delayUs(200); // Wait > 100us

    // 3rd time
    write4bits(0x03);
    _hal->delayUs(200); // Wait > 100us

    // 4th time. Send 0x02 - THIS SWITCHES THE LCD TO 4-BIT MODE
    write4bits(0x02);
    _hal->delayUs(100);

    // Now we are in 4-BIT mode
    // Now we can use sendCommand(), which safely splits commands into two nibbles
    // Step C: send full function_set command(DL=0, N=1, and F=0)
    uint8_t functionCmd = LCD_FUNCTIONSET | LCD_4BITMODE | LCD_5x8DOTS;

    functionCmd |= (rows>1)?LCD_TWOLINE:LCD_ONELINE;
    sendCommand(functionCmd);
    
    // Step D: Turn Display ON, Cursor OFF, BLINK OFF
    displayOn();

    // Step E: Clear Display (takes 1.52ms, handled inside clear())
    clear();

    // Step F: Set Entry Mode
    sendCommand(LCD_ENTRYMODESET | LCD_INCREMENT);
}

// 4. WRITE4BITS() - THE LOWEST HARDWARE LEVELS

// concept 3: Bitwise shifting (>>) and Bitwise AND (&)
// We need to extract 4 bits from the 'value' and put them on the pins
void MyLCD::write4bits(uint8_t value){
    for (int i=0; i<4; i++)
    {   
        uint8_t bitState = (value >> i) & 0x01;
        _hal->digitalWrite(_data_pins[i], bitState);
    }
    pulseEnable();
}

void MyLCD::pulseEnable(){
    _hal->digitalWrite(_en, 0);
    _hal->delayUs(1);
    _hal->digitalWrite(_en, 1);
    _hal->delayUs(1);
    _hal->digitalWrite(_en, 0);
    _hal->delayUs(100);
}

void MyLCD::send(uint8_t value, uint8_t mode){
    
    _hal->digitalWrite(_rs, mode);
    write4bits(value>>4);
    write4bits(value & 0x0F);
}

void MyLCD::sendCommand(uint8_t cmd)
{
    send(cmd, 0);
}

void MyLCD::sendData(uint8_t data){
    send(data,1);
}
void MyLCD::clear(){
    sendCommand(LCD_CLEARDISPLAY);
    _hal->delayMs(2);
}
void MyLCD::writeChar(uint8_t c){
    sendData(c);
}

void MyLCD::setCursor(uint8_t col, uint8_t row)
{
    if(_rows ==0 || _cols == 0) return ;
    if (row >= _rows) row = _rows - 1;
    if (col >= _cols) col = _cols - 1;
    static const uint8_t rowOffset[] = {0x00, 0x40, 0x14, 0x54};
    if(row > 3) row = 3;
    sendCommand(LCD_SETDDRAMADDR | (rowOffset[row] + col));
}

void MyLCD::displayOn()
{   
    _displayControl |= LCD_DISPLAYON;
    sendCommand(LCD_DISPLAYCONTROL | _displayControl);
}
void MyLCD::displayOff()    
{   
    _displayControl &= ~LCD_DISPLAYON;
    sendCommand(LCD_DISPLAYCONTROL | _displayControl);
}
void MyLCD::cursorOn()
{
    _displayControl |= LCD_CURSORON;
    sendCommand(LCD_DISPLAYCONTROL | _displayControl);
}
void MyLCD::cursorOff()
{
    _displayControl &= ~LCD_CURSORON;
    sendCommand(LCD_DISPLAYCONTROL | _displayControl);
}
void MyLCD::blinkOff()
{
    _displayControl &= ~LCD_BLINKON;
    sendCommand(LCD_DISPLAYCONTROL | _displayControl);
}
void MyLCD::blinkOn()
{
    _displayControl |= LCD_BLINKON;
    sendCommand(LCD_DISPLAYCONTROL | _displayControl);
}
void MyLCD::createChar(uint8_t slot, const uint8_t *pattern)
{
    if (slot > 7) return;
    sendCommand(LCD_SETCGRAMADDR | (slot * 8));
    for (uint8_t i = 0; i<8 ; i++)
        sendData(pattern[i]);
    sendCommand(LCD_SETDDRAMADDR);
}
void MyLCD::home()
{
    sendCommand(LCD_RETURNHOME);
    _hal->delayMs(2);
}
void MyLCD::print(const char* str)
{
    if (!str) return; // Guard against nullptr
    while(*str)
    {
        writeChar(*str);
        str++;
    }
}