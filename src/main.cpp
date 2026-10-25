// Include libraries
#include <Arduino.h>
#include "MyLCD.h"

// Pin Definitions
const uint8_t PIN_RS = 12;
const uint8_t PIN_EN = 11;
const uint8_t PIN_D4 = 5;
const uint8_t PIN_D5 = 4;
const uint8_t PIN_D6 = 3;
const uint8_t PIN_D7 = 2;

// LCD DIMENSIONS
const uint8_t LCD_ROWS = 2;
const uint8_t LCD_COLS = 16;

// Custom characters (CGRAM) demo data
const uint8_t patterns[8][8] = {
    {0x00,0x0A,0x1F,0x1F,0x1F,0x0E,0x04,0x00},  // heart
    {0x00,0x00,0x0A,0x0A,0x0A,0x11,0x0E,0x00},  // sad face
    {0x00,0x00,0x0A,0x00,0x11,0x0E,0x00,0x00},  // wink
    {0x1F,0x11,0x11,0x11,0x11,0x11,0x11,0x1F},  // box
    {0x00,0x0E,0x11,0x11,0x11,0x11,0x0E,0x00},  // circle
    {0x04,0x0A,0x11,0x11,0x11,0x11,0x0A,0x04},  // diamond
    {0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F},  // filled block
    {0x00,0x01,0x02,0x14,0x08,0x14,0x02,0x01},  // random arrow-ish
};
// Arduino Wrapper Functions
// 1. Pin Mode
void arduino_pinMode(uint8_t pin, uint8_t mode)
{
  // mode: input=0, output=1;
  pinMode(pin, mode);
}

// 2. Digital Write
void arduino_digitalWrite(uint8_t pin, uint8_t level){
  // Level: Low = 0, High = 1;
  digitalWrite(pin, level);
}

// 3. Delay us
void arduino_delayus(uint32_t us){
  // delay in us
  delayMicroseconds(us);
}

// 4. Delay ms
void arduino_delayms(uint32_t ms){
  // delay in ms
  delay(ms);
}

// Creating HAL struct
LCD_HAL_t myHAL = {
  .pinMode = arduino_pinMode,
  .digitalWrite = arduino_digitalWrite,
  .delayUs = arduino_delayus,
  .delayMs = arduino_delayms,
};

// Creating LCD object
MyLCD lcd(&myHAL, PIN_RS, PIN_EN, PIN_D4, PIN_D5, PIN_D6, PIN_D7);

void setup(){
  // Initialize LCD
  lcd.begin(LCD_COLS, LCD_ROWS);
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Hello Peeps !!");

  lcd.setCursor(0,1);
  lcd.print("It worked.");

  // Custom character (CGRAM) demo - uncomment to try it:
  // for (uint8_t i = 0; i < 8; i++) {
  //   lcd.createChar(i, patterns[i]);
  // }
  // lcd.setCursor(0, 0);
  // for (uint8_t i = 0; i < 8; i++) {
  //   lcd.writeChar(i);
  // }

  delay(5000);
}

void loop(){
  lcd.cursorOn();
  lcd.blinkOn();
  for (uint8_t i = 0; i < LCD_COLS; i++)
  {
    lcd.setCursor(i,0);
    lcd.writeChar('a' + i);
    delay(500);
  }
  delay(1000);
  lcd.cursorOff();
  for (uint8_t i = 0; i < LCD_COLS; i++)
  {
    lcd.setCursor(i,1);
    lcd.writeChar('A' + i);
    delay(500);
  }
  delay(1000);
  lcd.clear();
}

