#include <ZumoShield.h>

ZumoMotors motors;
ZumoReflectanceSensorArray sensors;
Pushbutton button(ZUMO_BUTTON);
ZumoBuzzer buzzer;

unsigned int sensorValues[6];
unsigned int linePos; // position of the line (readLine function)

int NORMAL_SPEED = 200; // speed: 0 (stop) - 400 (full)
int LEFT_SPEED;
int RIGHT_SPEED;

void setup() {
  // put your setup code here, to run once:
  
  // Initialize the sensors
  sensors.init();

  // calibration
  
 
  // wait for the user to press the button
  // button.waitForButton();
}

void loop() {
  linePos = sensors.readLine(sensorValues);  
  // 0 - directly below sensor 0 
  // 1 - directly below sensor 1
  // 5 - 5000

  if (linePos == 2500) {
    LEFT_SPEED = NORMAL_SPEED;
    RIGHT_SPEED = NORMAL_SPEED;
  }
  else if (linePos < 2500){
    LEFT_SPEED 
  }
  }
}
  


  
  // put your main code here, to run repeatedly:
  motors.setSpeeds(SPEED, SPEED);
  delay(10000);
  SPEED = 0;
