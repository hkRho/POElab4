#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *myMotor1 = AFMS.getMotor(1); // Right Motor
Adafruit_DCMotor *myMotor2 = AFMS.getMotor(2); // Left Motor

int RSPin = 0;
int LSPin = 0;
int RSvalue = 0;
int LSvalue = 0;
int motor1Speed;
int motor2Speed;
int turn_delay = 10;


void setup() {
  AFMS.begin();
  motor1Speed = Serial.read();
  motor2Speed = Serial.read();
//  myMotor1->setSpeed(50);
//  myMotor2->setSpeed(50);
  Serial.begin(9600);

}

void calibration() {
  
}

void loop() {
  RSvalue = analogRead(A0);
  LSvalue = analogRead(A1);

  myMotor1->setSpeed(0);
  myMotor2->setSpeed(0);

  Serial.print("Right Value: ");
  Serial.print(RSvalue);
  Serial.print (" | Left Value: ");
  Serial.println(LSvalue);

  
//  if (RSvalue > x && LSvalue > x){
//    forward();
//  }

//  if (RSvalue > x && LSvalue > x){
//    left();
//  }

//  if (RSvalue > x && LSvalue > x){
//    right();
//  }

//  if (RSvalue > x && LSvalue > x){
//    forward ();
//  }

}

// Both left and right sensor detects line
void forward(){
  myMotor1->run(FORWARD);
  myMotor2->run(FORWARD);
  delay(turn_delay);
}

// Left sensor detects line but right sensor does not
void left() {
  // if move left, right motor moves
  myMotor1->run(FORWARD);
  myMotor2->run(RELEASE);
  delay(turn_delay);
}

// Right sensor detects line but left sensor does not
void right() {
  myMotor1->run(RELEASE);
  myMotor2->run(FORWARD);
  delay(turn_delay);
}
