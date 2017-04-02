// Constants.
static float MINSERVO   = 546;    //minimum servo arc measured
static float MAXSERVO   = 2403;   //maximum servo arc measured
static float MAXSPEED   = 500;    //maximum Roomba forward velocity
static float MINSPEED   = -500;   //minimum Roomba reverse velocity
static float MAXRADIUS  = 2000;   //maximum Roomba turn radius
static float MINRADIUS  = -2000;  //minimum Roomba turn radius
static int MAXINPUT     = 50;     //max receiving buffer size

// PAN/TILT Servo INIT.
//Servo panServo;
//Servo tiltServo;
int panPos        = 1500;
int panSpeed      = 0;
int tiltPos       = 1500;
int tiltSpeed     = 0;
int panServoPin   = 2;
int tiltServoPin  = 3;

// Roomba INIT.
//Roomba r(2, 23);

int roombaDDPin   = 23;
int roombaSpeed   = 0;
int roombaRadius  = 0;

// Laser Init.
int laserTogglePin    = 22;
int laserState        = 0;     //0 is off, 1 is on.
int laserTargetState  = 0; //0 is off, 1 is on.


////////////////////////////////////////////////////////////////////////////////