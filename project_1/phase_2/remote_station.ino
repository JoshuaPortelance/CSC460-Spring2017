/*
 *  Project_1 - Jakob Roberts & Joshua Portelance
 *
 *  remote_station.ino
 *
 *  This program will run on the remote station arduino for project 1.
 *
 *  The setup will include:
 *      2 servos
 *      1 light sensor
 *      1 laser
 *      1 bluetooth module
 *
================================================================================
 *  PORT LIST
 *  ==Serial==
 *  Serial1 :   "bluetooth"
 *  Serial2 :   "roomba"
 *  ==Analog==
 *  A0      :   lightSensorPin
 *  ==PWM==
 *  PWM2    :   panServoPin
 *  PWM3    :   tiltServoPin
 *  ==DGTL(22-53)==
 *  DGTL22  :   laserPin
 *  DGTL23  :   roombaDDPin
 */

#include "scheduler.h"
#include "Roomba_Driver.h"
#include <Servo.h>

// Constants.
static float MINSERVO   = 546;    //minimum servo arc measured
static float MAXSERVO   = 2403;   //maximum servo arc measured
static float MAXSPEED   = 500;    //maximum roomba forward velocity
static float MINSPEED   = -500;   //minimum roomba reverse velocity
static float MAXRADIUS  = 2000;   //maximum roomba turn radius
static float MINRADIUS  = -2000;  //minimum roomba turn radius
static int MAXINPUT     = 50;     //max receiving buffer size

// Debuging.
int idle_debug_pin          = 32;
int laser_debug_pin         = 34;
int update_servo_debug_pin  = 36;
int update_roomba_debug_pin = 38;
int check_light_debug_pin   = 40;
int transmit_debug_pin      = 42;
int receive_debug_pin       = 44;

// PAN/TILT Servo INIT.
Servo panServo;
Servo tiltServo;
int panPos        = 1500;
int panSpeed      = 0;
int tiltPos       = 1500;
int tiltSpeed     = 0;
int panServoPin   = 2;
int tiltServoPin  = 3;

// Roomba INIT.
Roomba r(2, 23);
int roombaDDPin   = 23;
int roombaSpeed   = 0;
int roombaRadius  = 0;

// Light Sensor Init.
int lightSensorPin    = A0;
int lightThreshold    = 500;
int lightSensorState  = 0; //0 NO LIGHT, 1 THERE IS LIGHT!

// Laser Init.
int laserTogglePin    = 22;
int laserState        = 0;     //0 is off, 1 is on.
int laserTargetState  = 0; //0 is off, 1 is on.

/*============================================================================*/
// Read in a update transmission.
void receiveTransmission()
{
  digitalWrite(receive_debug_pin, HIGH);
  int status = 0;
  
  // Packet #.
  unsigned int packetnum = 0;
  
  // Expected packet values.
  char panSpeed_val [MAXINPUT];
  char tiltSpeed_val [MAXINPUT];
  char laser_val [MAXINPUT];
  char roombaSpeed_val [MAXINPUT];
  char roombaRadius_val [MAXINPUT];

  // Cycle through char array.
  unsigned int input_pos = 0;
  
  while(Serial1.available()){
    
    const byte inByte = Serial1.read();
    
    // FOR DEBUG.
    Serial.write(inByte);
    
    if(inByte == '#' || status == 1)
    {
      if(inByte == '#')
      {
        status = 1;
      }
      else if(inByte == '%')
      {
        // ADD NULL TERMINATORS.
        switch(packetnum)
        {
          case 0:
            panSpeed_val      [input_pos] = '\0';  // terminating null byte
            break;
          case 1:
            tiltSpeed_val     [input_pos] = '\0';  // terminating null byte
            break;
          case 2:
            laser_val         [input_pos] = '\0';  // terminating null byte
            break;
          case 3:
            roombaSpeed_val   [input_pos] = '\0';  // terminating null byte
            break;
          case 4:
            roombaRadius_val  [input_pos] = '\0';  // terminating null byte
            break;
          default:
            break;
        }
        
        // PROCESS INPUT VALUES.
        panSpeed          = atoi(panSpeed_val);
        tiltSpeed         = atoi(tiltSpeed_val);
        laserTargetState  = atoi(laser_val);
        roombaSpeed       = atoi(roombaSpeed_val);
        roombaRadius      = atoi(roombaRadius_val);
        
        // Reset values.
        input_pos = 0;
        packetnum = 0;
        status = 0;
        
      }
      else if(inByte == '|')
      {
        // ADD NULL TERMINATORS.
        switch(packetnum)
        {
          case 0:
            panSpeed_val      [input_pos] = '\0';  // terminating null byte
            break;
          case 1:
            tiltSpeed_val     [input_pos] = '\0';  // terminating null byte
            break;
          case 2:
            laser_val         [input_pos] = '\0';  // terminating null byte
            break;
          case 3:
            roombaSpeed_val   [input_pos] = '\0';  // terminating null byte
            break;
          case 4:
            roombaRadius_val  [input_pos] = '\0';  // terminating null byte
            break;
          default:
            break;
        }
        input_pos = 0;
        packetnum++;
      }
      else
      {
        switch(packetnum)
        {
          case 0:
            if (input_pos < (MAXINPUT - 2))
              panSpeed_val[input_pos++] = inByte;
              break;
          case 1:
            if (input_pos < (MAXINPUT - 2))
              tiltSpeed_val[input_pos++] = inByte;
              break;
          case 2:
            if (input_pos < (MAXINPUT - 2))
              laser_val[input_pos++] = inByte;
              break;
          case 3:
            if (input_pos < (MAXINPUT - 2))
              roombaSpeed_val[input_pos++] = inByte;
              break;
          case 4:
            if (input_pos < (MAXINPUT - 2))
              roombaRadius_val[input_pos++] = inByte;
              break;
          default:
            break;
        }
      }
    }
  }
  digitalWrite(receive_debug_pin, LOW);
}

/*============================================================================*/
// Send an update transmission.
// ~0.05ms average runtime
void sendTransmission()
{
  digitalWrite(transmit_debug_pin, HIGH);
  Serial1.print('#' + String(lightSensorState) + "%\n");
  digitalWrite(transmit_debug_pin, LOW);
}

/*============================================================================*/
// Update the positions of the servos.
// ~ 0.08ms average runtime
void updateServos()
{
  digitalWrite(update_servo_debug_pin, HIGH);
  panPos  += panSpeed;
  if(panPos>MAXSERVO)panPos = MAXSERVO;
  if(panPos<MINSERVO)panPos = MINSERVO;
  
  tiltPos += tiltSpeed;
  if(tiltPos>MAXSERVO)tiltPos = MAXSERVO;
  if(tiltPos<MINSERVO)tiltPos = MINSERVO;
    
  panServo.writeMicroseconds(panPos);
  tiltServo.writeMicroseconds(tiltPos);
  digitalWrite(update_servo_debug_pin, LOW);
}

/*============================================================================*/
// Update the state of the laser.
// ~ 0.05ms average runtime
void updateLaser()
{
  digitalWrite(laser_debug_pin, HIGH);
  if (laserState != laserTargetState)
  {
    laserState = laserTargetState;
    if (laserTargetState == 0)
      digitalWrite(laserTogglePin, LOW);
    if (laserTargetState == 1)
      digitalWrite(laserTogglePin, HIGH);
  }
  digitalWrite(laser_debug_pin, LOW);
}

/*============================================================================*/
// Update the Roomba.
/* Go straight  - 32768 for radious
 * Stop         - 0 speed, 0 radius
 * Radius should be between -5 to 5
 * Speed should be between -500 to 500
 */
void updateRoomba()
{
  digitalWrite(update_roomba_debug_pin, HIGH);
  if(roombaSpeed>MAXSPEED)roombaSpeed = MAXSPEED;
  if(roombaSpeed<MINSPEED)roombaSpeed = MINSPEED;
    
  if(roombaRadius>MAXRADIUS)roombaRadius = MAXRADIUS;
  if(roombaRadius<MINRADIUS)roombaRadius = MINRADIUS;
    
  r.drive(roombaSpeed, roombaRadius);
  digitalWrite(update_roomba_debug_pin, LOW);
}

/*============================================================================*/
// Check is there is light hitting the photo resistor.
// ~0.15ms average runtime
void checkLightSensor()
{
  digitalWrite(check_light_debug_pin, HIGH);
  if (analogRead(lightSensorPin) > lightThreshold)
    lightSensorState = 1;
  else
    lightSensorState = 0;
  digitalWrite(check_light_debug_pin, LOW);
}

/*============================================================================*/
// Idle function. Run any low-priority quick code here.
void idle(uint32_t idle_period)
{
  // This is just pulsing a pin so we can check when we enter idle.
  digitalWrite(idle_debug_pin, HIGH);
  delay(idle_period);
  digitalWrite(idle_debug_pin, LOW);
}

/*============================================================================*/
// Setup
void setup()
{
  // PIN INIT//////////////////////
  pinMode(idle_debug_pin, OUTPUT);
  pinMode(laser_debug_pin, OUTPUT);
  pinMode(update_servo_debug_pin, OUTPUT);
  pinMode(update_roomba_debug_pin, OUTPUT);
  pinMode(transmit_debug_pin, OUTPUT);
  pinMode(receive_debug_pin, OUTPUT);
  pinMode(check_light_debug_pin, OUTPUT);
  
  pinMode(laserTogglePin, OUTPUT);
  
  // Serial INIT///////////////////
  Serial1.begin(9600);
  Serial.begin(9600);

  // Roomba INIT///////////////////
  r.init();
 
  // Scheduler INIT////////////////
  Scheduler_Init();
  
  panServo.attach(panServoPin);
  tiltServo.attach(tiltServoPin);
 
  // Start task arguments are:
  // Scheduler_StartTask(start offset in ms, period in ms, function);
  Scheduler_StartTask(0, 30, receiveTransmission);
  Scheduler_StartTask(3, 20, updateServos);
  Scheduler_StartTask(6, 100, updateRoomba);
  Scheduler_StartTask(9, 100, updateLaser);
  Scheduler_StartTask(12, 200, checkLightSensor);
  Scheduler_StartTask(15, 200, sendTransmission);
}

/*============================================================================*/
// Program Loop
void loop()
{
  uint32_t idle_period = Scheduler_Dispatch();
  if (idle_period)
  {
    idle(idle_period);
  }
}

/*============================================================================*/
// Main control loop of the program.
int main()
{
  init();
  setup();
 
  for (;;)
  {
    loop();
  }
  for (;;);
  return 0;
}