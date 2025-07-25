// imports
#include <Wire.h>
#include "MT6701.h"
#include <Arduino.h>
#include <L293D.h>

// some definitions
#define MOTOR_A      20   // motor pin (A0)
#define MOTOR_B      21   // motor pin (A1)
#define MOTOR_ENABLE 29   // pwm pin   (B1)
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
}

void loop() {
  static float prev_angle = 0.0; // static kept alive between loops

  // read angular position
  float curr_angle = encoder.angleRead();

  // find the current ang vel
  float omega = prev_angle - curr_angle;

  // set motor speed
  motor.SetMotorSpeed(100);

  // update prev angle
  prev_angle = curr_angle;

  delay(10);
}
