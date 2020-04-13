#include <ZumoShield.h>

ZumoMotors motors;

// speed: 0 (stop) - 400 (full)
int SPEED = 200;

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  motors.setSpeeds(SPEED, SPEED);
  delay(5000);
  SPEED = 0;
}
