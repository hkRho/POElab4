#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *RightMotor = AFMS.getMotor(1); // Right Motor
Adafruit_DCMotor *LeftMotor = AFMS.getMotor(2); // Left Motor

int RSPin = 0;
int LSPin = 0;
int RSvalue = 0;
int LSvalue = 0;
int turn_delay = 10;
int base_speed = 30;

int RmotorSpeed;
int LmotorSpeed;
unsigned int currentError;
unsigned int previousError = 0;
unsigned long currentMillis;
unsigned long previousMillis = 0;

int k1 = 0.02;
int k2 = 0.03;

void setup() {
  AFMS.begin();
  Serial.begin(9600);
}


void loop() {
  RSvalue = analogRead(A0);
  LSvalue = analogRead(A1);
  currentError = LSvalue - (RSvalue+6);

  // left value is  > than right by 12~20
  Serial.print("Right Value: ");
  Serial.print(RSvalue);
  Serial.print (" | Left Value: ");
  Serial.println(LSvalue);

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

  RmotorSpeed = base_speed + (k1*currentError + k2*((currentError-previousError)/(currentMillis - previousMillis)));
  LmotorSpeed = base_speed - (k1*currentError + k2*((currentError-previousError)/(currentMillis - previousMillis)));
  
  RightMotor->setSpeed(RmotorSpeed);
  LeftMotor->setSpeed(LmotorSpeed);
  RightMotor->run(FORWARD);
  LeftMotor->run(FORWARD);
  
  previousMillis = currentMillis;
  previousError = currentError;
}
