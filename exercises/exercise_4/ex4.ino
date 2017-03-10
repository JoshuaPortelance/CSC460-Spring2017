//Sample using LiquidCrystal library
#include <LiquidCrystal.h>

/*******************************************************

This program will test the LCD panel and the buttons

Originally written by:
Mark Bramwell, July 2010

Modified by Joshua Portelance, January 2017 for CSC 460

********************************************************/

// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// define some values used by the panel and buttons
float freq        = 135;
int lcd_key     = 0;
int adc_key_in  = 0;
int count       = 0;
int lastButton  = 5;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

// read the buttons
int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 // For V1.1 us this threshold
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 250)  return btnUP; 
 if (adc_key_in < 450)  return btnDOWN; 
 if (adc_key_in < 650)  return btnLEFT; 
 if (adc_key_in < 850)  return btnSELECT;  

 // For V1.0 comment the other threshold and use the one below:
/*
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 195)  return btnUP; 
 if (adc_key_in < 380)  return btnDOWN; 
 if (adc_key_in < 555)  return btnLEFT; 
 if (adc_key_in < 790)  return btnSELECT;   
*/


 return btnNONE;  // when all others fail, return this...
}

void setup()
{
 lcd.begin(16, 2);              // start the library
 lcd.setCursor(0,0);
 lcd.print("Freq: " + String(1000.0/freq) + "Hz"); // print a simple message
 lcd.setCursor(0,1); 
 lcd.print("NONE");
}
 
void loop()
{
 lcd.setCursor(9,1);            // move cursor to second line "1" and 9 spaces over
 lcd.print(count);      // display seconds elapsed since power-up

 lcd.setCursor(0,1);            // move to the begining of the second line
 lcd_key = read_LCD_buttons();  // read the buttons

 /*
  * This is filtering out bad sampling frequencies.
  * We need to find the right frequency so we do not need this if statement.
  */
 /*
 if(lastButton != lcd_key)
 {
    count++;
    lastButton = lcd_key;
 }
 */

 switch (lcd_key)               // depending on which button was pushed, we perform an action
 {
   case btnRIGHT:
     {
     lcd.print("RIGHT ");
     count++;
     break;
     }
   case btnLEFT:
     {
     lcd.print("LEFT   ");
     count++;
     break;
     }
   case btnUP:
     {
     lcd.print("UP    ");
     count++;
     break;
     }
   case btnDOWN:
     {
     lcd.print("DOWN  ");
     count++;
     break;
     }
   case btnSELECT:
     {
     lcd.print("SELECT");
     count++;
     break;
     }
 }

 delay(freq);
}
