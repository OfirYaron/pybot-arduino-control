#include <Servo.h>

// Servo1: Gripper - Closing-Openinng
#define SERVO1_PIN 3
#define SERVO1_NEUTRAL 1500  // Servo neutral position Gripped angle
#define SERVO1_MIN_PULSEWIDTH 1700 //Open
#define SERVO1_MAX_PULSEWIDTH 2400 //Closed
#define SERVO1_RANGE (SERVO1_MAX_PULSEWIDTH-SERVO1_MIN_PULSEWIDTH)

// Servo2: Grip Angel
#define SERVO2_PIN 4
#define SERVO2_TOTAL_ANGEL 180
#define SERVO2_NEUTRAL 1500  // Servo neutral position
#define SERVO2_MIN_PULSEWIDTH 500
#define SERVO2_MAX_PULSEWIDTH 3000
#define SERVO2_RANGE (SERVO2_MAX_PULSEWIDTH-SERVO2_MIN_PULSEWIDTH)
#define SPTR_SIZE   20

// Height Stepper (A)
#define STEPA_STEP_PIN 5
#define STEPA_DIR_PIN 2
#define STEPS_PER_REV 200

//iCH4 = ExtractParamInt2b(10); // servo1 => orientation
//iCH5 = ExtractParamInt2b(12); // servo2 => gripper

Servo servo1; // Wrist orientation
Servo servo2; // Gripper open/close

char   *sPtr [SPTR_SIZE];
//int16_t actual_angleA1;
//int16_t actual_angleA2;
//int16_t actual_valueZ = 0;
int16_t actual_valueW = 0;
int16_t actual_valueG = 0;
//int16_t actual_distance = 0;

long servo_counter;
int16_t servo_pos1;
int16_t servo_pos2;
bool servo1_ready = false;
bool servo2_ready = false;

int separate (String str, char   **p, int    size ){
    int  n;
    char s [100];

    strcpy (s, str.c_str ());

    *p++ = strtok (s, " ");
    for (n = 1; NULL != (*p++ = strtok (NULL, " ")); n++)
        if (size == n)
            break;

    return n;
}

void enableServo1()
{
  servo1_ready=true;
  servo1.attach(SERVO1_PIN);
}

void enableServo2()
{
  servo2_ready=true;
  servo2.attach(SERVO2_PIN);
}

void moveServo1(int pwm)
{
  pwm = constrain(pwm,SERVO1_MIN_PULSEWIDTH,SERVO1_MAX_PULSEWIDTH);
  servo1.writeMicroseconds(pwm);
  actual_valueW = pwm;
}

// move servo2 on OC4A (pin13)
void moveServo2(int pwm)
{
  pwm = constrain(pwm,SERVO2_MIN_PULSEWIDTH,SERVO2_MAX_PULSEWIDTH);
  servo2.writeMicroseconds(pwm);
  actual_valueG = pwm;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(SERVO1_PIN, OUTPUT);
  pinMode(SERVO2_PIN, OUTPUT);
  pinMode(STEPA_STEP_PIN, OUTPUT);
  pinMode(STEPA_DIR_PIN,OUTPUT);

  enableServo1();
  enableServo2();

  Serial.begin(9600);
}

void stepper(bool up, int rev) {
  Serial.print("\nmoving up");  
  if (up) {
    Serial.print("\nmoving up");
    digitalWrite(STEPA_DIR_PIN,HIGH);
  } else {
    Serial.print("\nmoving down");
    digitalWrite(STEPA_DIR_PIN,LOW);
  }

  for(int x=0; x<(STEPS_PER_REV*rev); x++) {
    digitalWrite(STEPA_STEP_PIN,HIGH);
    delayMicroseconds(1000);
    digitalWrite(STEPA_STEP_PIN,LOW);
    delayMicroseconds(1000);    
  }
}

int Servo2Angel(int degree) {
  return SERVO2_MIN_PULSEWIDTH + (SERVO2_RANGE/SERVO2_TOTAL_ANGEL*degree);
}

void closeGripper(bool close) {
  if (close) {
    Serial.print("\nclosing tool");
    moveServo1(SERVO1_MAX_PULSEWIDTH);
  } else {
    Serial.print("\nopenning tool");
    moveServo1(SERVO1_MIN_PULSEWIDTH);    
  }
}

void changeToolAngle(int angle) {
  Serial.print("\nchanging tool angle to " + String(angle));
  moveServo2(Servo2Angel(angle));  
}

void invoke(char *command) {
//  Serial.println(command);
  String commandStr = String(command);

  if (command[0] == 'o') { //open
    closeGripper(false);    
  } else if (command[0] == 'c') { //close
    closeGripper(true);
  } else if (command[0] == 'u') { //up
    stepper(true, commandStr.substring(1).toInt());
  } else if (command[0] == 'd') { //down
    stepper(false, commandStr.substring(1).toInt());
  } else if (command[0] == 'a') { //tool angle
    changeToolAngle(commandStr.substring(1).toInt());
  } else if (command[0] == 's') { //stop - delay
    Serial.print("\ndelay for " + commandStr.substring(1).toInt());
    delay(commandStr.substring(1).toInt());
  } else {
    Serial.print("\nUNRECOGNIZED COMMAND " + commandStr);
  }
  delay(1000);
}

void loop() {
  while (Serial.available() == 0) {}
  String commandLine = Serial.readString();
  commandLine.trim();
  int N = separate (commandLine, sPtr, SPTR_SIZE);

  Serial.print("\nStarting Sequence...");
  for (int n = 0; n < N; n++) {
    invoke(sPtr [n]);
  }
  Serial.print("\nSequence Done.");

  delay(1000);
}
