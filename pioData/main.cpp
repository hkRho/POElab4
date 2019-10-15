#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>

void setMotorSpeed(int RMspeed, int LMspeed);
void decideDirection(int RSvalue, int LSvalue);
void recvWithEndMarker();
void parseNewData();
void printData();

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *rightMotor = AFMS.getMotor(1); // Right Motor
Adafruit_DCMotor *leftMotor = AFMS.getMotor(2); // Left Motor

int RSvalue = 0;
int LSvalue = 0;
int Mspeed = 0;
int effectiveLMS = 0;
int effectiveRMS = 0;
unsigned long ct = 0;

// Serial Input Work
const byte numChars = 32;
char caseData = 'p';
char inputData[numChars]; // an array to store the received data

boolean newData = false;

// Override var - true when directly setting motor speeds, and not via PID
boolean oride = false;
// Motor Speed | Left sensor threshold | Right sensor threshold | Motor override (stop)
int cvals[4] = {60,900,870,0};

void setup() {
  AFMS.begin();
  Serial.begin(9600);
}

void loop() {
  RSvalue = analogRead(A0);
  LSvalue = analogRead(A1);

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
    rightMotor->run(FORWARD);
    leftMotor->run(FORWARD);
		effectiveLMS = Mspeed;
		effectiveRMS = Mspeed;
  }
  else if (LSvalue > cvals[1] && RSvalue <= cvals[2]) {
    rightMotor->run(RELEASE);
    leftMotor->run(FORWARD);
		effectiveLMS = Mspeed;
		effectiveRMS = 0;
  }
  else if (LSvalue <= cvals[1] && RSvalue > cvals[2]) {
    rightMotor->run(FORWARD);
    leftMotor->run(RELEASE);
		effectiveLMS = 0;
		effectiveRMS = Mspeed;
  }
	printData();
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
        Serial.println(zoomy);
        cvals[0] = zoomy;
        break;
      case 'l': case 'L':
        Serial.println(zoomy);
        cvals[1] = zoomy;
      case 'r': case 'R':
        Serial.println(zoomy);
        cvals[2] = zoomy;
      case 's': case 'S':
        cvals[3] = 1;
        break;
      case 'g': case 'G':
        cvals[3] = 0;
        break;
      default:
        cvals[0] = zoomy;
        cvals[1] = zoomy;
        cvals[2] = zoomy;
        cvals[3] = zoomy;
        break;
    }
  }
}

void printData() {
		String delim = ",";
		ct = millis();
		String s = ct+delim+LSvalue+delim+RSvalue+delim+effectiveLMS+delim+effectiveRMS;
		Serial.println(s);
}
