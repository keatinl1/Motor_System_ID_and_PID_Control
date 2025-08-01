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
#define WC           50  // rad/s
#define KP  0.0038
#define KI  0.0254
#define KD  6.58*pow(10, -5)

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

float angle_to_ang_vel(float prev_angle, float curr_angle) {
  // edge case handling
  if (prev_angle > 180.0 && curr_angle < 180.0) {
    prev_angle = prev_angle - 360;
  }
  float delta_angle = curr_angle - prev_angle;
  return delta_angle / DT;
}

float lpf(float omega_x_k, float omega_x_k_1, float omega_y_k_1) {
  // low pass filter, with cutoff frequency
  float alpha = (2-DT*WC) / (2+DT*WC);
  float gamma = DT*WC / (2 + DT*WC);
  return alpha*omega_y_k_1 + gamma*(omega_x_k + omega_x_k_1);
}

float pid(float error, float sum, float rate) {
  float input = KP*error + KI*sum + KD*rate;

  if (input > 5.0){
    input = 5.0;
  }
  if (input < -5.0){
    input = -5.0;
  }

  return input;
}

void loop() {
  if (silent_timer.hasTimedOut()) {

    static int step_counter = 0;
    static float ref = 0;

    if (step_counter < 500) {
      ref = 0;
    } else if (step_counter < 1000) {
      ref = 12500;
    } else if (step_counter < 1500) {
      ref = 25000;
    } else if (step_counter < 2000) {
      ref = 12500;
    } else if (step_counter < 2500) {
      ref = 0;
    } else {
      step_counter = 0;
    }
    step_counter++;


    // get angular velocity
    static float prev_angle = 0.0;
    float curr_angle = encoder.angleRead();

    // estimate angular velocity with a filter
    static float prev_raw_omega = 0.0; // xk-1
    float raw_omega = angle_to_ang_vel(prev_angle, curr_angle); // xk
    static float prev_omega = 0.0; // yk-1
    float omega = lpf(raw_omega, prev_raw_omega, prev_omega);

    // control
    static float prev_error = 0;
    static float sum = 0;
    
    float error = ref - omega;
    sum += error*DT;
    float rate = (error-prev_error)/DT;
    float input = pid(error, sum, rate);

    motor.SetMotorSpeed(input); // voltage input

    // print for serial monitor
    Serial.print("omega:");
    Serial.println(omega);
    Serial.print("ref:");
    Serial.println(ref);
    Serial.print("x0:");
    Serial.println(0.0);

    // update vars
    prev_angle = curr_angle;
    prev_raw_omega = raw_omega;
    prev_omega = omega;
    prev_error = error;
    silent_timer.reset();
  }
}
