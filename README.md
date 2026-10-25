# MyLCD — HD44780 Character LCD Driver (HAL-based)

A small, learning-oriented driver for **HD44780-compatible character LCDs** used
in 4-bit mode. It is written as a reusable PlatformIO library with a clean
**Hardware Abstraction Layer (HAL)**: the library never calls `pinMode`,
`digitalWrite`, or `delay*` directly — you inject those functions. This makes the
core portable beyond Arduino and easy to unit-test on a desktop.

> This is a *learning* version: the code is heavily commented to explain the
> HD44780 initialization sequence, nibble shifting, and command masks.

## Features

- HD44780-compatible character LCDs (16x2, 20x4, ...)
- 4-bit interface (4 data pins + RS + EN)
- HAL abstraction — inject pin I/O and time delays via function pointers
- Basic command set: `clear()`, `home()`, display/cursor/blink on-off
- `print()` strings and `writeChar()` single characters
- CGRAM support: up to 8 custom 5x8 characters via `createChar()`
- Bounds-safe `setCursor()` for 2-line and 4-line row addressing
- No dependency on the Arduino framework inside the library (only `<stdint.h>`)

## Hardware requirements

- Any HD44780-based character LCD (e.g. 1602A, 2004A)
- A 5 V logic MCU (the demo targets an Arduino Uno)
- A 10 k&Omega; potentiometer for the V0 contrast pin

### Wiring (4-bit mode)

The demo sketch uses these default pins (customizable via the constructor):

| LCD pin | Function | Connect to  |
|---------|----------|-------------|
| 1 (VSS) | Ground   | GND         |
| 2 (VDD) | +5 V     | 5V          |
| 3 (V0)  | Contrast | middle pin of 10 k&Omega; pot |
| 4 (RS)  | Register select | Arduino **D12** |
| 5 (RW)  | Read/Write | GND (write-only) |
| 6 (E)   | Enable   | Arduino **D11** |
| 7 (D4)  | Data bit 4 | Arduino **D5** |
| 8 (D5)  | Data bit 5 | Arduino **D4** |
| 9 (D6)  | Data bit 6 | Arduino **D3** |
| 10 (D7) | Data bit 7 | Arduino **D2** |
| 15 (A)  | Backlight anode | 5V via 220 &Omega; resistor |
| 16 (K)  | Backlight cathode | GND |

## Project structure

```
MyLCD/
├── lib/MyLCD/          # the reusable library
│   ├── MyLCD.h         # API + HD44780 command/mask constants
│   └── MyLCD.cpp       # implementation
├── src/main.cpp        # demo sketch (Arduino Uno)
└── platformio.ini      # PlatformIO config (atmelavr / Uno / Arduino framework)
```

## Getting started

1. Install [PlatformIO](https://platformio.org/) (CLI or VS Code extension).
2. Build and upload the demo:

   ```sh
   pio run -t upload
   ```

3. Watch the LCD: it prints **"Hello Peeps !!"** / **"It worked."**, then sweeps
   lowercase letters across row 0 and uppercase letters across row 1, with a
   blinking cursor.

## How the HAL works

`MyLCD` only knows about a HAL contract — a struct of four function pointers:

```cpp
typedef struct LCD_HAL_t {
    void (*pinMode)(uint8_t pin, uint8_t mode);      // LCD_INPUT / LCD_OUTPUT
    void (*digitalWrite)(uint8_t pin, uint8_t level); // LCD_LOW   / LCD_HIGH
    void (*delayUs)(uint32_t us);
    void (*delayMs)(uint32_t ms);
} LCD_HAL_t;
```

On Arduino you implement the wrappers and hand them over:

```cpp
void arduino_pinMode(uint8_t pin, uint8_t mode)      { pinMode(pin, mode); }
void arduino_digitalWrite(uint8_t pin, uint8_t level){ digitalWrite(pin, level); }
void arduino_delayus(uint32_t us)                    { delayMicroseconds(us); }
void arduino_delayms(uint32_t ms)                    { delay(ms); }

LCD_HAL_t myHAL = {
    .pinMode = arduino_pinMode,
    .digitalWrite = arduino_digitalWrite,
    .delayUs = arduino_delayus,
    .delayMs = arduino_delayms,
};

MyLCD lcd(&myHAL, /*RS*/12, /*EN*/11, /*D4*/5, /*D5*/4, /*D6*/3, /*D7*/2);

void setup() {
    lcd.begin(16, 2);   // 16 columns, 2 rows
    lcd.print("Hello, world!");
}

void loop() {}
```

To run the same library on another platform or in a PC test harness, simply
provide different functions — the LCD logic stays untouched.

## API reference

| Method | Description |
|--------|-------------|
| `MyLCD(hal, rs, en, d4, d5, d6, d7)` | Construct with a HAL pointer + pin numbers |
| `begin(cols, rows)` | Initialize the LCD — **must be called first** |
| `clear()` | Clear the display and return the cursor home |
| `home()` | Return the cursor to (0, 0) without clearing |
| `setCursor(col, row)` | Move the cursor (col 0..cols-1, row 0..rows-1) |
| `writeChar(c)` | Write one character at the cursor position |
| `print(str)` | Write a null-terminated string |
| `displayOn()` / `displayOff()` | Turn the display on/off (content is preserved) |
| `cursorOn()` / `cursorOff()` | Show / hide the cursor underline |
| `blinkOn()` / `blinkOff()` | Blink the current cursor cell |
| `createChar(slot, pattern)` | Store a 5x8 custom character into CGRAM slot 0-7 |

> **Note:** `setCursor(col, row)` takes the **column first**, then the row —
> e.g. second line, first cell is `setCursor(0, 1)`.

## Custom characters (CGRAM)

Each slot is an 8-byte bitmap, 5 bits per row; a set bit is a lit pixel. Example
(a heart):

```cpp
const uint8_t heart[8] = {
    0b00000,
    0b01010,
    0b11111,
    0b11111,
    0b11111,
    0b01110,
    0b00100,
    0b00000,
};

lcd.createChar(0, heart);   // load the pattern into slot 0
lcd.writeChar(0);           // display the custom character
```

## Implementation notes

- **Write-only driver** — the RW pin is tied to GND, so the library uses the
  fixed delays recommended by the HD44780 datasheet instead of polling the busy
  flag. This is simple and reliable, at the cost of a few hundred microseconds
  per call.
- **4-bit initialization** follows the official sequence `0x3, 0x3, 0x3, 0x2`
  (with a >= 50 ms settle after power-on) before the full `FunctionSet` command.
- **Display-control state** is kept as flag bits (display/cursor/blink) and the
  base opcode is OR-ed at send time, so `displayOff()`, `cursorOff()`, and
  `blinkOff()` are safe to call in any order.
- **Row addressing** uses the standard base offsets
  `{0x00, 0x40, 0x14, 0x54}` for 2- and 4-line displays; out-of-range
  coordinates are clamped.

## References

- HD44780 datasheet (Hitachi)
- Typical 16x2/20x4 LCD module datasheet (e.g. 1602A, 2004A)
- [PlatformIO documentation](https://docs.platformio.org/en/latest/)

## License

No license file is present yet. If you plan to publish or reuse this outside of
learning, add a license (e.g. MIT) at the project root.