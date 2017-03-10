#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

volatile int minutes   = 0;
volatile int seconds   = 0;
volatile int hSeconds  = 0;

////////////////////////////////////////////////////
ISR(TIMER3_COMPA_vect)
{
  digitalWrite(52, 1);
  
  hSeconds++;
  
  if (hSeconds >= 1000) {
    hSeconds = 0;
    seconds++;
  }
  
  if (seconds >= 60) {
    seconds = 0;
    minutes++;
  }
  
  if (minutes >= 60) {
    minutes = 0;
    seconds = 0;
    hSeconds = 0;
  }

  digitalWrite(52, 0);
}

////////////////////////////////////////////////////
void setup() {

  lcd.begin(16, 2); // start the library (init 16 per row, 2 rows)
  lcd.setCursor(0, 0);
  lcd.print("==INITIALIZING==");

  pinMode(52, OUTPUT);
  
  cli();  // disable global interrupts
  
  //Clear timer config.
  TCCR3A = 0;
  TCCR3B = 0;
  
  //Set to CTC (mode 4)
  TCCR3B |= (1<<WGM32);
  
  // Set CS10 bit so timer runs at clock speed:
  TCCR3B |= B00000011;
  
  //Setting timer target
  OCR3A = 250;
  
  //Enable interrupt A for timer 3.
  TIMSK3 |= (1<<OCIE3A);
  
  //Set timer to 0 (optional here).
  TCNT3 = 0;
  
  sei();  // enable global interrupts

  delay(1000);
  lcd.clear();
}

////////////////////////////////////////////////////
void loop() {
  // put your main code here, to run repeatedly:
  lcd.clear();
  lcd.home();
  lcd.print(String(minutes) + ":" + String(seconds) + ":" + String(hSeconds));
  delay(200);
}