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
    
    // change in angle to rad divide by change in const timestep
    float omega = delta_angle * 4.36332313;

    Serial.print("omega: ");
    Serial.print(omega);
    Serial.println("rad/s");    

    motor.SetMotorSpeed(3.0); // voltage input

    prev_angle = curr_angle;
    silent_timer.reset();
  }
}
