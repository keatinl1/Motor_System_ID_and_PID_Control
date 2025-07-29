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
#define DT           0.004
#define WC           25

// instantiate objects
L293D motor(MOTOR_A, MOTOR_B, MOTOR_ENABLE);
MT6701 encoder;
SoftTimer silent_timer;

void setup() {
  Serial.begin(115200);
  encoder.initializeI2C();
  motor.begin(true);
  Wire.begin();
  silent_timer.setTimeOutTime(4); // 4ms timeout = 250Hz
  silent_timer.reset();
}

float angle_to_ang_vel(float prev_angle, float curr_angle){
  // change in angular position with edge case
  if (prev_angle > 180.0 && curr_angle < 180.0) {
    prev_angle = prev_angle - 360;
  }
  float delta_angle = curr_angle - prev_angle;
  return delta_angle / DT;
}

float lpf(float omega_x_k, float omega_x_k_1, float omega_y_k_1){
  float alpha = (2-DT*WC) / (2+DT*WC);
  float gamma = DT*WC / (2 + DT*WC);
  return alpha*omega_y_k_1 + gamma*(omega_x_k + omega_x_k_1);
}

void loop() {
  if (silent_timer.hasTimedOut()) {
    // get angular velocity
    static float prev_angle = 0.0;
    float curr_angle = encoder.angleRead();

    static float prev_raw_omega = 0.0; // xk-1
    float raw_omega = angle_to_ang_vel(prev_angle, curr_angle); // xk
    static float prev_omega = 0.0; // yk-1

    // filter angular velocity

    float omega = lpf(raw_omega, prev_raw_omega, prev_omega);

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
    prev_raw_omega = raw_omega;
    prev_omega = omega;
    silent_timer.reset();
  }
}
