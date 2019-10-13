#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *rightMotor = AFMS.getMotor(1); // Right Motor
Adafruit_DCMotor *leftMotor = AFMS.getMotor(2); // Left Motor

int RSpin = 0;
int LSpin = 0;
int RSvalue = 0;
int LSvalue = 0;
int RMspeed = 30;
int LMspeed = 30;
int turn_delay = 10;

// Serial Input Work
const byte numChars = 32;
char caseData = 'p';
char inputData[numChars]; // an array to store the received data

boolean newData = false;

// Override var - true when directly setting motor speeds, and not via PID
boolean oride = false;
int cvals[4] = {0,0,0,0};

void setup() {
  AFMS.begin();
  Serial.begin(9600);
  Serial.println("<Arduino is ready>");
}

void loop() {
  RSvalue = analogRead(A0);
  LSvalue = analogRead(A1);
  Serial.print("Right Value: ");
  Serial.print(RSvalue);
  Serial.print (" | Left Value: ");
  Serial.println(LSvalue);
  
  // Read Serial input, if any
  recvWithEndMarker();
  parseNewData();
  RMspeed = cvals[0];
  LMspeed = cvals[1];
  
  setMotorSpeed(30,30);
  decideDirection(RSvalue, LSvalue);
}

void setMotorSpeed(int RMspeed, int LMspeed) {
  rightMotor->setSpeed(RMspeed);
  leftMotor->setSpeed(LMspeed);
}

void decideDirection(int RSvalue, int LSvalue) {
  if (RSvalue > 850 && LSvalue > 880){
    Serial.print("CASE1");
    // move forward
    rightMotor->run(FORWARD);
    leftMotor->run(FORWARD);
  }

  if (RSvalue > 815 && RSvalue < 850 && LSvalue > 840 && LSvalue < 880){
    // move left
    Serial.print("CASE2");
    rightMotor->run(FORWARD);
    leftMotor->run(RELEASE);
  }

  if (RSvalue > 800 && RSvalue < 815 && LSvalue > 810 && LSvalue < 840){
    // move right
    Serial.print("CASE3");
    rightMotor->run(RELEASE);
    leftMotor->run(FORWARD);
  }
}

void recvWithEndMarker() {
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;
  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();
    if (rc != endMarker) {
      if (ndx == 0) {
        caseData = rc;
      }
      else {
        inputData[ndx-1] = rc;
      }
      ndx++;
      if (ndx > numChars) {
        ndx = numChars - 1;
      }
    }
    else {
      inputData[ndx-1] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}

void parseNewData() {
  if (newData == true) {
    // Debugs input values
    // Serial.print("This just in ... ");
    // Serial.println(inputData);
    
    int zoomy = atoi(inputData);
    newData = false;
    switch(caseData) {
      case 'p': case 'P':
        Serial.print("Proportional input");
        Serial.println(zoomy);
        cvals[0] = zoomy;
        break;
      case 'd': case 'D':
        Serial.println("Derivate input");
        Serial.println(zoomy);
        cvals[1] = zoomy;
        break;
      case 'l': case 'L':
        oride = true;
        Serial.println("Left motor input");
        Serial.println(zoomy);
        cvals[2] = zoomy;
        break;
      case 'r': case 'R':
        Serial.println("Right motor input");
        Serial.println(zoomy);
        cvals[3] = zoomy;
        break;
      case 's': case 'S':
        Serial.println("O god o fuck");
        cvals[0] = zoomy;
        cvals[1] = zoomy;
        cvals[2] = zoomy;
        cvals[3] = zoomy;
        break;
      default:
        Serial.println("Something screwed up.");
        cvals[0] = zoomy;
        cvals[1] = zoomy;
        cvals[2] = zoomy;
        cvals[3] = zoomy;
        break;
    }
  }
}
