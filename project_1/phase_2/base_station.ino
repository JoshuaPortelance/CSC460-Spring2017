/*
  Project_1 - Jakob Roberts & Joshua Portelance
*/
/*============================================================================*/
/* PORT LIST
 **** Analog
 * A9       rightJoyXPin
 * A10      rightJoyYPin
 * A11      leftJoyYPin
 * A12      leftJoyXPin
 **** DGTL 22-53
 * DGTL18 TX1 Bluetooth send SERIAL1
 * DGTL19 RX1 Bluetooth recieve SERIAL1
 * DGTL22   rightJoySWPin
 * DGTL23   leftJoySWPin //NOT IN USE
 * TEST PINS:
 * DGTL30 bt_getData
 * DGTL31 bt_sendData
 * DGTL32 checkLeftJoyAxis
 * DGTL33 checkRightJoyAxis
 * DGTL34 getLaserSpeed
 * DGTL35 getRoombaSpeed
 * DGTL36 LCDprint
 * DGTL37 rightJoySwitch
 */
////////////////////////////////////////////////////////////////////////////////
/*============================================================================*/
#include<Servo.h>
#include <LiquidCrystal.h>
#include <scheduler.h>
/*============================================================================*/
int DEBUG = 0;
int idle_pin = 29;
int i; //looper
int MAXINPUT = 50;

//INIT LCD
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

//Roomba movement INIT
int radiusRoomba = 0;
int speedRoomba  = 0;

// Pan/Tilt speed INIT
int panSpeed  = 0;
int tiltSpeed = 0;

//Laser INIT
int laserState      = 0;   //0 is off, 1 is on.

//Joystick MIN/MAX Variables
int joyMax = 1024;
int joyMin = 0;

// Right Joystick INIT
int rightJoyXPin     = A10;
int rightJoyYPin     = A9;
int rightJoySWPin    = 22;
int rightXAxis       = 0;
int rightYAxis       = 0;
int rightXAxisCenter = 0;
int rightYAxisCenter = 0;

// Left  Joystick INIT
int leftJoyXPin     = A12;
int leftJoyYPin     = A11;
//int leftJoySWPin  = 23;
int leftXAxis       = 0;
int leftYAxis       = 0;
int leftXAxisCenter = 0;
int leftYAxisCenter = 0;

/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  LCD_start(); //LCD INIT
  Scheduler_Init(); //Initialize scheduler
  pinMode(rightJoySWPin, INPUT_PULLUP); //Init pullup res. 4 rightjoySWpin.
  pinMode(idle_pin, OUTPUT);

  //testing pins for scheduler
  pinMode(30, OUTPUT);
  digitalWrite(30, LOW);
  pinMode(31, OUTPUT);
  digitalWrite(31, LOW);
  pinMode(32, OUTPUT);
  digitalWrite(32, LOW);
  pinMode(33, OUTPUT);
  digitalWrite(33, LOW);
  pinMode(34, OUTPUT);
  digitalWrite(34, LOW);
  pinMode(35, OUTPUT);
  digitalWrite(35, LOW);
  pinMode(36, OUTPUT);
  digitalWrite(36, LOW);
  pinMode(37, OUTPUT);
  digitalWrite(37, LOW);

  //INIT Joystick Centers
  getJoyCenter();
  delay(2000);
  

  // Add Scheduled Tasks to Scheduler
  /* Scheduler_StartTask(int16_t delay, int16_t period, task_cb task);
   * TASKS:
   * bt_getData()         - 0.05ms (probably more if data exists)
   * bt_sendData()        - 0.54ms
   * checkLeftJoyAxis()   - 0.27ms
   * checkRightJoyAxis()  - 0.27ms
   * getLaserSpeed()      - 0.08ms
   * getRoombaSpeed()     - 0.08ms
   * LCDprint()           - 23.68ms
   * rightJoySwitch       - 0.03825ms
   */
  // BT Get Data
  Scheduler_StartTask(2, 50, bt_getData);
  // BT Send Data
  Scheduler_StartTask(6, 50, bt_sendData);
  // checkLeftJoyAxis()
  Scheduler_StartTask(8, 50, checkLeftJoyAxis);
  // checkRightJoyAxis()
  Scheduler_StartTask(10, 50, checkRightJoyAxis);
  // getLaserSpeed()
  Scheduler_StartTask(12, 50, getLaserSpeed);
  // getRoombaSpeed()
  Scheduler_StartTask(14, 50, getRoombaSpeed);
  // Print out LCD
  Scheduler_StartTask(0, 500, LCDprint);
  //rightJoySwitch()
  Scheduler_StartTask(0, 135, rightJoySwitch);
}
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
// Idle function. Run any low-priority quick code here.
// This is just pulsing a pin so we can check when we enter idle.
void idle(uint32_t idle_period){ 
  digitalWrite(idle_pin, HIGH);
  delay(idle_period);
  digitalWrite(idle_pin, LOW);
}
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
void loop() {
  uint32_t idle_period = Scheduler_Dispatch();
  if (idle_period){
    idle(idle_period);
  }
}
/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
// Main control loop of the program.
int main(){
  init();
  setup();
  for (;;){
    loop();
  }
  for (;;);
  return 0;
}



/* Blutooth Protocol Information
 * # beginning delimiter
 * | separation delimiter
 */

/* Alert if Light sensor has been triggered */
void bt_getData(){
  digitalWrite(30, HIGH);
  int status = 0;
  //packet#s
  static unsigned int packetnum = 0;
  //expected packet values
  char light_state [MAXINPUT];

  //cycle through char array
  unsigned int input_pos = 0;
  
  while(Serial1.available()){
    const byte inByte = Serial1.read();

    //debug
    Serial.write(inByte);
    
    if(inByte == '#' || status == 1){
      if(inByte == '#'){
        status = 1;
      }else if(inByte == '%'){
        //ADD NULL TERMINATORS
        switch(packetnum){
          case 0:
            light_state [input_pos] = '\0';  // terminating null byte
            break;
          default:
            break;
        }
        
        //PROCESS INPUT VALUES
        if(atoi(light_state) == 1){
          lightSensorKill();
        }

        //reset values
        input_pos = 0;
        packetnum = 0;
        status = 0;
      }else if(inByte == '|'){
        switch(packetnum){
          case 0:
            light_state [input_pos] = '\0';  // terminating null byte
            break;
          default:
            break;
        }
        input_pos = 0;
        packetnum++;
      }else{
        switch(packetnum){
          case 0:
            if (input_pos < (MAXINPUT - 2))
              light_state[input_pos++] = inByte;
            break;
          default:
            break;
        }
      }
    }
  }
  digitalWrite(30, LOW);
}

/* Sends data on interval
 * 
 * -panSpeed
 * -tiltSpeed
 * -laserState
 * -roombaSpeed (-500 to 500)
 * -roombaRadius (-2000 to +2000
 * 
 * eg:  #5|0|1|
 * panning at speed 5, tilt off, laser on, 
 */
void bt_sendData(){
  digitalWrite(31, HIGH);
  //write to Serial1
  Serial1.print("#"+String(panSpeed)+"|"+String(tiltSpeed)+"|"+String(laserState)+"|"+String(speedRoomba)+"|"+String(-radiusRoomba)+"%\n");
//  Serial.print("#"+String(panSpeed)+"|"+String(tiltSpeed)+"|"+String(laserState)+"|"+String(speedRoomba)+"|"+String(-radiusRoomba)+"%\n");
  digitalWrite(31, LOW);
}



/*============================================================================*/
// INITIALIZES Joystick Centers
void getJoyCenter() {
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
}
/*============================================================================*/
// SCHEDULED
void getLaserSpeed() {
  digitalWrite(34, HIGH);
  panSpeed = -(rightXAxis - rightXAxisCenter);
  panSpeed = panSpeed/90;

  tiltSpeed = -(rightYAxis - rightYAxisCenter);
  tiltSpeed = tiltSpeed/90;
  digitalWrite(34, LOW);
}
/*============================================================================*/
// SCHEDULED
void getRoombaSpeed() {
  digitalWrite(35, HIGH);
  int radiusOffset = 40;
  //0 is straight
  //2000 (+and-) are next increment, (+/-)1 is fastest spin
  radiusRoomba = -(leftXAxis - leftXAxisCenter);
  if(radiusRoomba < radiusOffset && radiusRoomba > -radiusOffset){
    radiusRoomba = 0;
  }else{
    radiusRoomba = radiusRoomba*4;
    
    if(radiusRoomba >  2000)radiusRoomba =  1999;
    if(radiusRoomba < -2000)radiusRoomba = -1999;

    if(radiusRoomba <  2000 && radiusRoomba >=  radiusOffset)radiusRoomba = 2000-radiusRoomba;
    if(radiusRoomba > -2000 && radiusRoomba <= -radiusOffset)radiusRoomba = -2000-radiusRoomba;
  }

  //(+/-)500
  int MAXSPEED = 3; //* 100 !!!
  int speedOffset = 30;
  float tempspeed=0.000;
  speedRoomba = -(leftYAxis - leftYAxisCenter);
  if(speedRoomba < speedOffset && speedRoomba > -speedOffset){
    speedRoomba = 0;
  }else{
    tempspeed = speedRoomba / 5; //(normal max is 500)
    speedRoomba = tempspeed * MAXSPEED;
    if(speedRoomba >  500)speedRoomba =  500;
    if(speedRoomba < -500)speedRoomba = -500;
  }
  digitalWrite(35, LOW);
}
/*============================================================================*/
// SCHEDULED
void rightJoySwitch() {
  digitalWrite(37, HIGH);
  if (digitalRead(rightJoySWPin) == 0) { //pushing button makes value go to 0
    if(laserState == 0){
      laserState = 1;
    }else{
      laserState = 0;
    }
  }    
  digitalWrite(37, LOW);
}
/*============================================================================*/
// SCHEDULED
void checkLeftJoyAxis() {
  digitalWrite(32, HIGH);
  leftXAxis = analogRead(leftJoyXPin);
  leftYAxis = analogRead(leftJoyYPin);
  if(leftXAxis<joyMin || leftXAxis>joyMax)errorDisplay("leftXAxis");
  if(leftYAxis<joyMin || leftYAxis>joyMax)errorDisplay("leftYAxis");
  digitalWrite(32, LOW);
}
/*============================================================================*/
// SCHEDULED
void checkRightJoyAxis() {
  digitalWrite(33, HIGH);
  rightXAxis = analogRead(rightJoyXPin);
  rightYAxis = analogRead(rightJoyYPin);
  if(rightXAxis<joyMin || rightXAxis>joyMax)errorDisplay("rightXAxis");
  if(rightYAxis<joyMin || rightYAxis>joyMax)errorDisplay("rightYAxis");
  digitalWrite(33, LOW);
}
/*============================================================================*/
void errorDisplay(String errormessage){
  lcd.setCursor(0, 0);
  lcd.print("ERROR: "+errormessage);
  return;
}


void LCD_start(){
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
}

void LCDprint(){
  digitalWrite(36, HIGH);

  if(DEBUG == 1){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(leftXAxis);
    lcd.setCursor(4, 0);
    lcd.print(radiusRoomba);
  //  lcd.print(rightXAxis);
    lcd.setCursor(0, 1);
    lcd.print(leftYAxis);
    lcd.setCursor(4, 1);
    lcd.print(speedRoomba);
  //  lcd.print(rightYAxis);
    lcd.setCursor(9, 0);
    lcd.print(digitalRead(rightJoySWPin));
    lcd.setCursor(10, 0);
    lcd.print(laserState);
    lcd.setCursor(12, 0);
    lcd.print(panSpeed);
    lcd.setCursor(14, 0);
    lcd.print(tiltSpeed);
  }

  if(DEBUG == 0){
    lcd.clear();
    if(laserState == 0){
      if(radiusRoomba == 0 && speedRoomba == 0){
        lcd.setCursor(0, 0);
        //---------################---
        lcd.print("IM THE SNEAKIEST");
        lcd.setCursor(0, 1);
        lcd.print(" VACUUM CLEANER ");
        delay(10);
      }else{
        lcd.setCursor(0, 0);
        //---------################---
        lcd.print("SEE ME ROOOMBAIN");
        lcd.setCursor(0, 1);
        lcd.print("THEY HATIN'.....");
        delay(10);
      }
    }else{
      lcd.setCursor(0, 0);
      //---------################---
      lcd.print("IMMMA FIRING MAH");
      lcd.setCursor(0, 1);
      lcd.print("LAYZOORRRRRRRRRR");
      delay(10);
    }
  }
  digitalWrite(36, LOW);
}

void lightSensorKill(){
  int s = 0;
  lcd.clear();
  while(1){
    if(s == 0){
      lcd.setCursor(0, 0);
      //---------################---
      lcd.print("LIGHT SENSOR HIT");
      lcd.setCursor(0, 1);
      lcd.print("~~ SHUT  DOWN ~~");
      delay(3000);
      s = 1;
    }else{
      lcd.setCursor(0, 0);
      //---------################---
      lcd.print("URGGGG HE GOT ME");
      lcd.setCursor(0, 1);
      lcd.print("I HAD GOOD LIFE~");
      delay(2000);
      s = 0;
    }
  }
  
}