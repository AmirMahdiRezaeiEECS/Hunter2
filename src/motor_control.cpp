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

void MotorController::setSpeed(int leftPercent, int rightPercent) {
  leftPercent = constrain(leftPercent, -100, 100);
  rightPercent = constrain(rightPercent, -100, 100);

  setMotorSpeeds(map(abs(leftPercent), 0, 100, 0, MAX_MOTOR_SPEED),
                 leftPercent > 0 ? HIGH : LOW,
                 leftPercent < 0 ? HIGH : LOW,
                 map(abs(rightPercent), 0, 100, 0, MAX_MOTOR_SPEED),
                 rightPercent > 0 ? HIGH : LOW,
                 rightPercent < 0 ? HIGH : LOW);
}

void MotorController::executeCommand(int command) {
  switch (command) {
    case 1: // Turn left
      setSpeed(0, 100);
      break;

    case 2: // Forward
      setSpeed(100, 100);
      break;

    case 3: // Turn right
      setSpeed(100, 0);
      break;

    case 4: // Sharp left
      setSpeed(-90, 90);
      break;

    case 5: // Stop
      stop();
      break;

    case 6: // Sharp right
      setSpeed(90, -90);
      break;

    case 7: // Reverse-left
      setSpeed(0, -100);
      break;

    case 8: // Reverse
      setSpeed(-90, -90);
      break;

    case 9: // Reverse-right
      setSpeed(-100, 0);
      break;

    default:
      stop();
      break;
  }
}
