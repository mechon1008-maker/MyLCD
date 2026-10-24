// Inlcude libraries
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
  // lcd.cursorOff();
  lcd.setCursor(0,0);

  const char* message = "Hello, HAL!";
  for(int i=0; message[i] != '\0'; i++){
    lcd.writeChar(message[i]);
  }
  delay(1000);
  lcd.clear();
}

void loop(){
  // lcd.clear();
  // lcd.displayOn();
  lcd.cursorOn();
  lcd.blinkOn();
  for (int i = 0; i<16; i++)
  {
    lcd.setCursor(0,i);
    lcd.writeChar('a' + i);
    delay(500);
    // lcd.clear();
  }
  delay(1000);
  lcd.cursorOff();
  for (int i = 0; i<16; i++)
  {
    lcd.setCursor(1,i);
    lcd.writeChar('A' + i);
    delay(500);
    // lcd.clear();
  }
  delay(1000);
  lcd.clear();
  // lcd.setCursor(0,0);
  // lcd.writeChar('A');
  // lcd.setCursor(1,2);
  // lcd.writeChar('B');

  // const char* line1 = "LINE 1";
  // for(int i=0; line1[i] != '\0'; i++)
  //   lcd.writeChar(line1[i]);
  // delay (1000);

  // lcd.setCursor(2,4);
  // const char* line2 = "LINE 2";
  // for(int i=0; line2[i] != '\0'; i++)
  //   lcd.writeChar(line2[i]);
  // delay(1000);
}

