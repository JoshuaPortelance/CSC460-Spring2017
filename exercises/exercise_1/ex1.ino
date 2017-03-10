/*
  Multple Serial test
 Receives from the main serial port, sends to the others.
 Receives from serial port 1, sends to the main serial (Serial 0).
 This example works only with boards with more than one serial like Arduino Mega, Due, Zero etc
 The circuit:
 * Any serial device attached to Serial port 1
 * Serial monitor open on Serial port 0:
 created 30 Dec. 2008
 modified 20 May 2012
 by Tom Igoe & Jed Roach
 modified 27 Nov 2015
 by Arturo Guadalupi
 This example code is in the public domain.
 */
int bytecount;
int inByte;
void setup() {
  // initialize both serial ports:
  Serial.begin(9600);
  Serial1.begin(9600);
  bytecount = 0;
}
void terminate(){
  while(1);
}
void loop() {
  // read from port 1, send to port 0:
  if (Serial1.available()) {
    inByte = Serial1.read();
    Serial.write(inByte);
    bytecount++;
  }
    // read from port 0, send to port 1:
  if (Serial.available()) {
    inByte = Serial.read();
    Serial1.write(inByte);
    if(bytecount %30 == 0 && bytecount != 0){
      Serial.print("\n");
    }
  }
  if(inByte == 35){
    Serial.print("\n====================\n");
    Serial.write("Number of Bytes: ");
    Serial.print(bytecount);
    terminate();
  }
  
}
