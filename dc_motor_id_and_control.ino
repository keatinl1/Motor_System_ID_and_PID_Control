// imports
#include <Wire.h>
#include "MT6701.h"
#include <Arduino.h>
#include <L293D.h>

// some definitions
#define MOTOR_A      20   // motor pin a
#define MOTOR_B      21   // motor pin b
#define MOTOR_ENABLE 29   // Enable (also PWM pin)
#define PWM_MOTOR_FREQUENCY   200
#define PWM_MOTOR_RESOLUTION    8

// instantiate objects
L293D motor(MOTOR_A, MOTOR_B, MOTOR_ENABLE);
MT6701 encoder;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  encoder.initializeI2C();
  motor.begin(true);
  motor.SetMotorSpeed(100);
}

void loop() {
  // read angular position
  float curr_angle = encoder.angleRead();
  Serial.println(curr_angle);
  
  // set motor speed


  delay(10);
}
