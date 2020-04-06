#include <ZumoShield.h>

ZumoMotors motors;
// speed: 0 (stop) -400 (full)
int INITIAL_SPEED = 100;

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  motors.setLeftSpeed(INITIAL_SPEED);
  motors.setRightSpeed(INITIAL_SPEED);
  delay(4000);
  INITIAL_SPEED = 0;
}
