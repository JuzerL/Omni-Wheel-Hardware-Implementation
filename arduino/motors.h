#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>

/*
  =====================================================
  L298N MOTOR DRIVER CONFIGURATION (4 MOTORS)
  =====================================================

  Each motor uses:
  - PWM (ENA/ENB) → speed control
  - IN1, IN2 → direction control

  Logic:
    speed > 0 → IN1=HIGH, IN2=LOW
    speed < 0 → IN1=LOW, IN2=HIGH
    speed = 0 → IN1=LOW, IN2=LOW (brake/stop)

  Speed range: -255 to 255
*/

/*
  ===========================
  PIN DEFINITIONS
  ===========================

  You can adjust these depending on your wiring.
*/

// -------- FRONT LEFT (Motor 1) --------
#define FL_PWM 5
#define FL_IN1 4
#define FL_IN2 3

// -------- FRONT RIGHT (Motor 2) --------
#define FR_PWM 6
#define FR_IN1 7
#define FR_IN2 8

// -------- REAR LEFT (Motor 3) --------
#define RL_PWM 9
#define RL_IN1 10
#define RL_IN2 11

// -------- REAR RIGHT (Motor 4) --------
#define RR_PWM 11   // change if conflict on your board
#define RR_IN1 12
#define RR_IN2 13


/*
  ===========================
  SETUP FUNCTION
  ===========================
*/
inline void setup_motors()
{
  pinMode(FL_PWM, OUTPUT);
  pinMode(FL_IN1, OUTPUT);
  pinMode(FL_IN2, OUTPUT);

  pinMode(FR_PWM, OUTPUT);
  pinMode(FR_IN1, OUTPUT);
  pinMode(FR_IN2, OUTPUT);

  pinMode(RL_PWM, OUTPUT);
  pinMode(RL_IN1, OUTPUT);
  pinMode(RL_IN2, OUTPUT);

  pinMode(RR_PWM, OUTPUT);
  pinMode(RR_IN1, OUTPUT);
  pinMode(RR_IN2, OUTPUT);
}


/*
  ===========================
  SINGLE MOTOR CONTROL
  ===========================
*/
inline void write_single_motor(int pwm, int in1, int in2, int speed)
{
  speed = constrain(speed, -255, 255);

  if (speed > 0)
  {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    analogWrite(pwm, speed);
  }
  else if (speed < 0)
  {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    analogWrite(pwm, -speed);
  }
  else
  {
    // Stop motor
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(pwm, 0);
  }
}


/*
  ===========================
  MAIN MOTOR FUNCTION
  ===========================
*/
inline void write_motors(int w1, int w2, int w3, int w4)
{
  // Mapping:
  // W1 = Front Left
  // W2 = Front Right
  // W3 = Rear Left
  // W4 = Rear Right

  write_single_motor(FL_PWM, FL_IN1, FL_IN2, w1);
  write_single_motor(FR_PWM, FR_IN1, FR_IN2, w2);
  write_single_motor(RL_PWM, RL_IN1, RL_IN2, w3);
  write_single_motor(RR_PWM, RR_IN1, RR_IN2, w4);
}

#endif
