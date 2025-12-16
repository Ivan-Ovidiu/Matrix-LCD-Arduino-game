#include <stdint.h>
#include "Arduino.h"
#include "LCD.h"

#define D4_PIN 2
#define D5_PIN 4
#define D6_PIN 6
#define D7_PIN 7
#define EN_PIN 9
#define RS_PIN 8
#define VO_PIN 5

// Brightness levels


float lcdBrightness = 85;
LiquidCrystal lcd(RS_PIN, EN_PIN, D4_PIN, D5_PIN, D6_PIN, D7_PIN);

void initialiseAssets() {
  uint8_t heart[8] = {B00000, B00000, B01010, B11111, B11111, B01110, B00100, B00000};
  lcd.createChar(0, heart);
  uint8_t shield[8] { B11111, B11011, B10001, B11011, B11111, B01110, B01110, B00000};
  lcd.createChar(1,shield);
}

void lcdInitialization() {
  lcd.begin(16, 2);
  pinMode(VO_PIN, OUTPUT);
  analogWrite(VO_PIN, lcdBrightness);
  initialiseAssets();
}


