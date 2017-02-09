/*
  Project_1 - Jakob Roberts & Joshua Portelance
*/
/*============================================================================*/
/* PORT LIST
 **** Analog
 * A8       lightSensorPin
 * A9       rightJoyXPin
 * A10      rightJoyYPin
 * A11      leftJoyXPin
 * A12      leftJoyYPin
 * A13      
 * A14      
 * A15 
 **** PWM
 * PWM2     panServoPin
 * PWM3     tiltServoPin
 * PWM4     
 * PWM5     
 * PWM6     
 * PWM7
 **** DGTL 22-53
 * DGTL22   rightJoySWPin
 * DGTL23   leftJoySWPin
 * DGTL24   laserTogglePin
 */

#include<Servo.h>
#include <LiquidCrystal.h>

//MIN & MAX over a 180 deg arc
float MINSERVO = 546;   //minimum servo arc measured
float MAXSERVO = 2403;  //maximum servo arc measured
int servoCurrent;       //last servo value used.

// PAN/TILT Servo INIT
Servo panServo;
Servo tiltServo;
int panPos = 1500;
int tiltPos = 1500;
int panServoPin = 2;
int tiltServoPin = 3;
int panSpeed = 0;
int tiltSpeed = 0;

//Light Sensor Init.
int lightSensorPin = A8;
int lightThreshold = 500;
int lightSensorState = 0; //0 is not enough, 1 THERE IS LIGHT!

//Joystick Variables
int joyMax = 1020;
int joyMin = 0;

// Right Joystick INIT
int rightJoyXPin = A9;
int rightJoyYPin = A10;
int rightJoySWPin = 22;
int rightXAxis = 0;
int rightYAxis = 0;
int rightXAxisCenter = 0;
int rightYAxisCenter = 0;

// Left  Joystick INIT
int leftJoyXPin = A11;
int leftJoyYPin = A12;
int leftJoySWPin = 23;
int leftXAxis = 0;
int leftYAxis = 0;
int leftXAxisCenter = 0;
int leftYAxisCenter = 0;

//Laser Init.
int laserTogglePin = 24;
int laserState = 0;   //0 is off, 1 is on.

//LCD Init.
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

//General Init.
int loopDelay = 135; //in milliseconds (for delay)
int i; //looper

int DEBUG = 1;

/*============================================================================*/
void setup() {
  // INIT pin 34 for HIGH/LOW input
  pinMode(34, OUTPUT);
  pinMode(laserTogglePin, OUTPUT);
  
  //Init pullup resistor on right joystick switch pin.
  pinMode(rightJoySWPin, INPUT_PULLUP);
  
  //LCD INIT//////////////////////
  lcd.begin(16, 2); // start the library (init 16 per row, 2 rows)
  lcd.setCursor(0, 0);
  lcd.print("-INITIALIZATION-");
  if(DEBUG==1){
    lcd.setCursor(0, 1);
    lcd.print("~~~~~DEBUG~~~~~~");
  }else{
    lcd.setCursor(0, 1);
    lcd.print("~~~~~~~~~~~~~~~~");
  }
  ////////////////////////////////
  
  panServo.writeMicroseconds(1500);
  tiltServo.writeMicroseconds(1500);
  delay(1000);
  
  //INIT JOYSTICK CENTERS
  int repeat = 30;
  for(i=0;i < repeat; i++){
    leftXAxisCenter  += analogRead(leftJoyXPin);
    leftYAxisCenter  += analogRead(leftJoyYPin);
    rightXAxisCenter += analogRead(rightJoyXPin);
    rightYAxisCenter += analogRead(rightJoyYPin);
    delay(10);
  }
  leftXAxisCenter  /= repeat;
  leftYAxisCenter  /= repeat;
  rightXAxisCenter /= repeat;
  rightYAxisCenter /= repeat;
  ///////////////////////
  
  panServo.attach(panServoPin);
  tiltServo.attach(tiltServoPin);

  //USE DEBUG STATE for LCD printout dbugging
  
  
  pinMode(laserTogglePin, OUTPUT);
  
  if(DEBUG == 0 || 1){
    lcd.clear();
  }else{
    // DISPLAY DEBUG VALUES
    lcd.clear();
  }

  //Initialize LCD.
  lcd.setCursor(8, 1);
  lcd.print("LIGHTOFF");
  
}

/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
void loop() {
  // digitalWrite(34, HIGH);
  lcd.clear();
  if(DEBUG==0){
    checkLightResistor();
    rightJoySwitch();
    
    
    
  }else{
    // checkLightResistor();
    // rightJoySwitch();
    
    checkLeftJoyAxis();
    checkRightJoyAxis();
    //Get Joystick Values
    // lcd.setCursor(0, 0);
    // lcd.print(panPos);
    // lcd.setCursor(4, 0);
    // lcd.print(tiltPos);
    // lcd.setCursor(0, 1);
    // lcd.print(rightXAxis);
    // lcd.setCursor(4, 1);
    // lcd.print(rightYAxis);
    // lcd.setCursor(8, 0);
    // lcd.print(digitalRead(rightJoySWPin));
    // lcd.setCursor(10, 0);
    // lcd.print(digitalRead(laserTogglePin));
    // lcd.setCursor(12, 0);
    // lcd.print(panSpeed);
    // lcd.setCursor(14, 0);
    // lcd.print(tiltSpeed);
    
    
    //~~~~JOYSTICK SPEED CALC AND APPLICATION~~~~
    
    //for Roomba
    if(leftXAxis<joyMin || leftXAxis>joyMax)errorDisplay("leftXAxis");
    if(leftYAxis<joyMin || leftYAxis>joyMax)errorDisplay("leftYAxis");
    
    //for Laser
    if(rightXAxis<joyMin || rightXAxis>joyMax)errorDisplay("rightXAxis");
    if(rightYAxis<joyMin || rightYAxis>joyMax)errorDisplay("rightYAxis");
    
    //X neg is left, pos is right
    panSpeed = (rightXAxis - rightXAxisCenter)/90;
    //Y neg is down, pos is up
    tiltSpeed = (rightYAxis - rightYAxisCenter)/90;
    
    panPos  += panSpeed*-10;
    if(panPos>MAXSERVO)panPos = MAXSERVO;
    if(panPos<MINSERVO)panPos = MINSERVO;
    
    tiltPos += tiltSpeed*10;
    if(tiltPos>MAXSERVO)tiltPos = MAXSERVO;
    if(tiltPos<MINSERVO)tiltPos = MINSERVO;
    
    
    panServo.writeMicroseconds(panPos);
    tiltServo.writeMicroseconds(tiltPos);
    
    
    
    
    
    
    
    
  }
  
  
  delay(loopDelay);
}
/* NOTES:
 * use the map-to function for the servos
 *
 */
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
//Checks if photo-resistor is being hit by a laser, updates LCD accordingly.
void checkLightResistor() {
  if (analogRead(lightSensorPin) > lightThreshold) {
      lightSensorState = 1;
      lcd.setCursor(8, 1);
      lcd.print("LIGHTON!");
  }
  else{
      lightSensorState = 0;
      lcd.setCursor(8, 1);
      lcd.print("LIGHTOFF");
  }
  return;
}

/*============================================================================*/
//Checks if the joystick switch is triggered.
void rightJoySwitch() {
    if (digitalRead(rightJoySWPin) == 0) { //pushing button makes value go to 0
        toggleLaser();
    }
    return;
}

/*============================================================================*/
//Toggles the laser on/off.
void toggleLaser() {
    if (laserState == 0) {
        laserState = 1;
        digitalWrite(laserTogglePin, HIGH);
    }
    else {
        laserState = 0;
        digitalWrite(laserTogglePin, LOW);
    }
    return;
}

/*============================================================================*/
//Checks if the left joystick moved.
void checkLeftJoyAxis() {
    leftXAxis = analogRead(leftJoyXPin);
    leftYAxis = analogRead(leftJoyYPin);
    return;
}

/*============================================================================*/
//Checks if the right joystick moved.
void checkRightJoyAxis() {
    rightXAxis = analogRead(rightJoyXPin);
    rightYAxis = analogRead(rightJoyYPin);
    return;
}

/*============================================================================*/
//Updates the position of the pan servo.
void updatePanServo(int target) {
  
}

/*============================================================================*/
//Updates the position of the tilt servo.
void updateTiltServo(int target) {
  
}
/*============================================================================*/
void errorDisplay(String errormessage){
  lcd.setCursor(0, 0);
  lcd.print("ERROR: "+errormessage);
  return;
}
