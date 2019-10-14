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
int Mspeed = 30;
int turn_delay = 10;

// Serial Input Work
const byte numChars = 32;
char caseData = 'p';
char inputData[numChars]; // an array to store the received data

boolean newData = false;

// Override var - true when directly setting motor speeds, and not via PID
boolean oride = false;
// Motor Speed | Left sensor threshold | Right sensor threshold | Motor override (stop)
int cvals[4] = {0,900,870,1};

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
  if (cvals[3] == 1) {
    Mspeed = 0;
  }
  else {
    Mspeed = cvals[0];
  }
  
  setMotorSpeed(Mspeed,Mspeed);
  decideDirection(RSvalue, LSvalue);
}

void setMotorSpeed(int RMspeed, int LMspeed) {
  rightMotor->setSpeed(RMspeed);
  leftMotor->setSpeed(LMspeed);
}

void decideDirection(int RSvalue, int LSvalue) {
  if (LSvalue > cvals[1] && RSvalue > cvals[2]) {
    Serial.println("Perfectly on line");
    rightMotor->run(FORWARD);
    leftMotor->run(FORWARD);
  }
  else if (LSvalue > cvals[1] && RSvalue <= cvals[2]) {
    Serial.println("Too left");
    rightMotor->run(RELEASE);
    leftMotor->run(FORWARD);
  }
  else if (LSvalue <= cvals[1] && RSvalue > cvals[2]) {
    Serial.println("Too right");
    rightMotor->run(FORWARD);
    leftMotor->run(RELEASE);
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
      case 'm': case 'M':
        oride = true;
        Serial.println("Motor speed:");
        Serial.println(zoomy);
        cvals[0] = zoomy;
        break;
      case 'l': case 'L':
        Serial.print("Set left sensor threshold: ");
        Serial.println(zoomy);
        cvals[1] = zoomy;
      case 'r': case 'R':
        Serial.print("Set left sensor threshold: ");
        Serial.println(zoomy);
        cvals[2] = zoomy;
      case 's': case 'S':
        Serial.println("O god o fuck");
        cvals[3] = 1;
        break;
      case 'g': case 'G':
        Serial.println("Gotta go fast");
        cvals[3] = 0;
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
