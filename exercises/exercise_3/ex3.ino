/*
EXERCISE_3 - Jakob Roberts & Joshua Portelance
*/

int ledPin = 13;    // LED connected to digital pin 9

void setup() {
  pinMode(34, OUTPUT); // enable pin 34
}

void loop() {
  // fade in from min to max in increments of 5 points:
  digitalWrite(34, HIGH);
  for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPin, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }

  // fade out from max to min in increments of 5 points:
  for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 5) {
    // sets the value (range from 0 to 255):
    analogWrite(ledPin, fadeValue);
    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }
  delay(100);
  digitalWrite(34, LOW);
  delay(100);
}


