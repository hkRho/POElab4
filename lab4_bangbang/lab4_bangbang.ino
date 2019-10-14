#include <Wire.h>
#include <Adafruit_MotorShield.h>
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *rightMotor = AFMS.getMotor(1); // Right Motor
Adafruit_DCMotor *leftMotor = AFMS.getMotor(2); // Left Motor

// initialize variable for right side sensor output value
int RSvalue = 0;

// initialize variable for left side sensor output value
int LSvalue = 0;

// initialize variable for the motor speed
int Mspeed = 30;

// initialize variable for determining whether to accept new data or not
boolean newData = false;

// Override var - true when directly setting motor speeds, and not via PID
boolean oride = false;

// Motor Speed | Left sensor threshold | Right sensor threshold | Motor override (stop)
int cvals[4] = {0,900,870,1};

// Serial Input Work
const byte numChars = 32;
char caseData = 'p';
char inputData[numChars]; // an array to store the received data


void setup() {
  AFMS.begin();
  Serial.begin(9600);
  Serial.println("<Arduino is ready>");
}


void loop() {
  // Grab sensor data
  RSvalue = analogRead(A0);
  LSvalue = analogRead(A1);
  printSensorOutput(RSvalue, LSvalue); // print sensor output to determine threshold
  
  // Read Serial input, if any
  recvWithEndMarker();
  parseNewData();

  // If override, set motor speed to 0
  if (cvals[3] == 1) {
    Mspeed = 0;
  }
  else {
    Mspeed = cvals[0];
  }

  // move the robot
  setMotorSpeed(Mspeed,Mspeed);
  decideDirection(RSvalue, LSvalue);
}

/// set motor speeds with the parameter
void setMotorSpeed(int RMspeed, int LMspeed) {
  rightMotor->setSpeed(RMspeed);
  leftMotor->setSpeed(LMspeed);
}


/// decide the direction to move depending on the RSvalue and LSvalue
void decideDirection(int RSvalue, int LSvalue) {
  // go forward
  if (LSvalue > cvals[1] && RSvalue > cvals[2]) {
    Serial.println("Perfectly on line");
    rightMotor->run(FORWARD);
    leftMotor->run(FORWARD);
  }
  // turn right
  else if (LSvalue > cvals[1] && RSvalue <= cvals[2]) {
    Serial.println("Too left");
    rightMotor->run(RELEASE);
    leftMotor->run(FORWARD);
  }
  // turn left
  else if (LSvalue <= cvals[1] && RSvalue > cvals[2]) {
    Serial.println("Too right");
    rightMotor->run(FORWARD);
    leftMotor->run(RELEASE);
  }
}


/// read all the data from serial input until a new line appears
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


/// uses serial connection to change the motor speed and right/left sensor threshold and stop/go
void parseNewData() {
  if (newData == true) {
    int zoomy = atoi(inputData);
    newData = false;
    
    switch(caseData) {
      case 'm': case 'M': // change motor speed
        oride = true;
        Serial.println("Motor speed:");
        Serial.println(zoomy);
        cvals[0] = zoomy;
        break;
      case 'l': case 'L': // change left sensor threshold
        Serial.print("Set left sensor threshold: ");
        Serial.println(zoomy);
        cvals[1] = zoomy;
      case 'r': case 'R': // change right sensor threshold
        Serial.print("Set right sensor threshold: ");
        Serial.println(zoomy);
        cvals[2] = zoomy;
      case 's': case 'S': // stop the robot
        cvals[3] = 1;
        break;
      case 'g': case 'G': // move the robot
        Serial.println("Gotta go fast");
        cvals[3] = 0;
        break;
      default:  // in case of error, set the input value to every element of cvals
        Serial.println("Something screwed up.");
        cvals[0] = zoomy;
        cvals[1] = zoomy;
        cvals[2] = zoomy;
        cvals[3] = zoomy;
        break;
    }
  }
}


/// print the IR sensor output values for testing
void printSensorOutput(int RSvalue, int LSvalue) {
  Serial.print("Right Value: ");
  Serial.print(RSvalue);
  Serial.print (" | Left Value: ");
  Serial.println(LSvalue);
}
