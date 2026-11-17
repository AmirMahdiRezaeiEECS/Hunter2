#include "motor_control.h"
#include "config.h"

void MotorController::begin() {
  pinMode(MOTOR_A_PWM, OUTPUT);
  pinMode(MOTOR_A_IN1, OUTPUT);
  pinMode(MOTOR_A_IN2, OUTPUT);

  pinMode(MOTOR_B_PWM, OUTPUT);
  pinMode(MOTOR_B_IN1, OUTPUT);
  pinMode(MOTOR_B_IN2, OUTPUT);

  stop();
}

void MotorController::setMotorSpeeds(
    uint8_t speedA, uint8_t inA1, uint8_t inA2,
    uint8_t speedB, uint8_t inB1, uint8_t inB2) {
  analogWrite(MOTOR_A_PWM, speedA);
  analogWrite(MOTOR_B_PWM, speedB);

  digitalWrite(MOTOR_A_IN1, inA1);
  digitalWrite(MOTOR_A_IN2, inA2);
  digitalWrite(MOTOR_B_IN1, inB1);
  digitalWrite(MOTOR_B_IN2, inB2);
}

void MotorController::stop() {
  setMotorSpeeds(0, LOW, LOW, 0, LOW, LOW);
}

void MotorController::executeCommand(int command) {
  switch (command) {
    case 1: // Turn left
      setMotorSpeeds(90, LOW, LOW, 100, HIGH, LOW);
      break;

    case 2: // Forward
      setMotorSpeeds(100, HIGH, LOW, 100, HIGH, LOW);
      break;

    case 3: // Turn right
      setMotorSpeeds(100, HIGH, LOW, 90, LOW, LOW);
      break;

    case 4: // Sharp left
      setMotorSpeeds(90, LOW, HIGH, 90, HIGH, LOW);
      break;

    case 5: // Stop
      stop();
      break;

    case 6: // Sharp right
      setMotorSpeeds(90, HIGH, LOW, 90, LOW, HIGH);
      break;

    case 7: // Reverse-left
      setMotorSpeeds(90, LOW, LOW, 100, LOW, HIGH);
      break;

    case 8: // Reverse
      setMotorSpeeds(90, LOW, HIGH, 90, LOW, HIGH);
      break;

    case 9: // Reverse-right
      setMotorSpeeds(100, LOW, HIGH, 90, LOW, LOW);
      break;

    default:
      stop();
      break;
  }
}
