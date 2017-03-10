/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  pinMode(22, OUTPUT);
  digitalWrite(22, LOW);   // turn the LED on (HIGH is the voltage level)
  myservo.write(90);
}

void loop() {
  digitalWrite(22, HIGH);
  
  for (pos = 90; pos <= 44; pos -= 1) { 
    // in steps of 1 degree
    myservo.write(pos);              
    delay(15);                       
  }

  digitalWrite(22, LOW);
  delay(500);
  digitalWrite(22, HIGH);
  
  for (pos = 44; pos <= 141; pos += 1) { 
    myservo.write(pos);              
    delay(15);                       
  }
  digitalWrite(22, LOW);
  delay(1000);
}

