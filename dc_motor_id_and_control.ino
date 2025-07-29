// imports
#include <Wire.h>
#include "MT6701.h"
#include <Arduino.h>
#include <L293D.h>
#include <SoftTimers.h>

// some definitions
#define MOTOR_A      20   // motor pin (A0)
#define MOTOR_B      21   // motor pin (A1)
#define MOTOR_ENABLE 29   // pwm pin   (B1)

// instantiate objects
L293D motor(MOTOR_A, MOTOR_B, MOTOR_ENABLE);
MT6701 encoder;
SoftTimer silent_timer;

void setup() {
  Serial.begin(115200);
  encoder.initializeI2C();
  motor.begin(true);
  Wire.begin();

  // Timer in "timed out" state
  silent_timer.setTimeOutTime(4); // 4ms timeout = 250Hz
  silent_timer.reset();
}

void loop() {
  if (silent_timer.hasTimedOut()) {
    // get angular position
    static float prev_angle = 0.0;
    float curr_angle = encoder.angleRead();

    // change in angular position with edge case
    if (prev_angle > 180.0 && curr_angle < 180.0) {
      prev_angle = prev_angle - 360;
    }
    float delta_angle = curr_angle - prev_angle;
    
    // change angle to deg/s by divide in const timestep
    float omega = delta_angle / 0.004;;

    Serial.print("omega:");
    Serial.println(omega);
    Serial.print("ref:");
    Serial.println(25000.0);

    float error = 25000 - omega;
    float input = 0.001 * error;

    if (input > 5.0){
      input = 5.0;
    }
    if (input < -5.0){
      input = -5.0;
    } 

    motor.SetMotorSpeed(input); // voltage input

    prev_angle = curr_angle;
    silent_timer.reset();
  }
}
