/*
EXERCISE_2 - Jakob Roberts & Joshua Portelance
*/

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
int i = 0; // for loop

void setup() {
  pinMode(34, OUTPUT); // enable pin 34
  Serial3.begin(9600); // Serial setup
  lcd.begin(16, 2);    // LCD setup

}

void loop() {
  delay(100); // delay between pulses
  digitalWrite(34, HIGH);
  for (i = 0; i < 100; i++) {
    //    Serial3.print("12345678901234567890123456789012");
    lcd.setCursor(0, 0);
    lcd.print("HelloWorldTester");
    lcd.setCursor(0, 1);
    lcd.print("THISisONLYaTEST!");
  }
  digitalWrite(34, LOW);
}

