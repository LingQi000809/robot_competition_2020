#include <ZumoShield.h>

ZumoMotors motors;
ZumoReflectanceSensorArray rSensors;


// speed: 0 (stop) - 400 (full)
int INITIAL_SPEED = 200;

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  motors.setSpeeds(INITIAL_SPEED, INITIAL_SPEED);
  delay(10000);
  INITIAL_SPEED = 0;
}
