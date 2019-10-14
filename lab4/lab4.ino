#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *RightMotor = AFMS.getMotor(1); // Right Motor
Adafruit_DCMotor *LeftMotor = AFMS.getMotor(2); // Left Motor

// Motor Code Variables

int RSPin = 0;
int LSPin = 0;
int RSvalue = 0;
int LSvalue = 0;
int turn_delay = 10;
int base_speed = 30;

int RmotorSpeed;
int LmotorSpeed;
int currentError;
int previousError = 0;
unsigned long currentMillis;
unsigned long previousMillis = 0;

int k1 = 0.02;
int k2 = 0.03;

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
  // Read Serial input, if any
  recvWithEndMarker();
  parseNewData();
  k1 = cvals[0];
  k2 = cvals[1];

  /// Do motor things
  RSvalue = analogRead(A0);
  LSvalue = analogRead(A1);
  currentError = LSvalue - (RSvalue+20);

  // left value is  > than right by 12~20
  Serial.print("Right Value: ");
  Serial.print(RSvalue);
  Serial.print (" | Left Value: ");
  Serial.print(LSvalue);
  Serial.print (" | Error: ");
  Serial.println(currentError);

  currentMillis = millis();
  /// move forward
  if (currentError < 3){
    RmotorSpeed = base_speed;
    LmotorSpeed = base_speed;
  }

//  /// stop the line-follower robot
//  if (error > 0 && error < 3){
//    RmotorSpeed = 0;
//    LmotorSpeed = 0;
//  }

  if(oride) {
    LmotorSpeed = cvals[2]; // Directly set left motor speed
    RmotorSpeed = cvals[3]; // Directly set right motor speed
  }
  else {
    RmotorSpeed = base_speed + (k1*currentError + k2*((currentError-previousError)/(currentMillis - previousMillis)));
    LmotorSpeed = base_speed - (k1*currentError + k2*((currentError-previousError)/(currentMillis - previousMillis)));
  }
  
  RightMotor->setSpeed(RmotorSpeed);
  LeftMotor->setSpeed(LmotorSpeed);
  RightMotor->run(FORWARD);
  LeftMotor->run(FORWARD);
  
  previousMillis = currentMillis;
  previousError = currentError;
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
        oride = true;
        cvals[0] = zoomy;
        cvals[1] = zoomy;
        cvals[2] = zoomy;
        cvals[3] = zoomy;
        break;
      case 'g': case 'G':
        Serial.println("O god o fuck");
        oride = false;
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
