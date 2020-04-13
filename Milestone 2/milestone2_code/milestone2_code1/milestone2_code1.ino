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
  buzzer.play(">g32>>c32");
  
  // Initialize the sensors
  sensors.init();

  // calibration by spinning
  delay(1000);
  int CALIBRATION_SPEED = 100;
  for (int t = 0; t < 80; t++){
    // 0-10s turn clockwise; 10-30s turn anti-clockwise; 30-50s turn clockwise; 50-70s turn anti-clockwise; 70-80 turn clockwise back to front
    if ((t <= 10) || (t > 30 && t <= 50) || (t>70)){
      motors.setSpeeds(CALIBRATION_SPEED, -CALIBRATION_SPEED);
    }
    else{
      motors.setSpeeds(-CALIBRATION_SPEED, CALIBRATION_SPEED);
    }
    sensors.calibrate();
    delay(20);
  }

//  // calibration by hand
//  for (int t =  0; t<100; t++){
//    sensors.calibrate();
//  }
  
  buzzer.play(">g32>>c32");
  motors.setSpeeds(0,0);
  // Wait for the user button to be pressed and released
  button.waitForButton();
  buzzer.play("L16 cegec4");
  while(buzzer.isPlaying());
}

void loop() {
  
  linePos = sensors.readLine(sensorValues);  
  // 0 - directly below sensor 0 
  // 1 - directly below sensor 1
  // 5 - 5000
  Serial.println(linePos);

  if (linePos > 3000) {
    // anti-clockwise
    LEFT_SPEED = NORMAL_SPEED;
    RIGHT_SPEED = 0;
  }
  else if (linePos < 2000){
    // clockwise
    LEFT_SPEED = 0;
    RIGHT_SPEED = NORMAL_SPEED;
  }
  else{
    LEFT_SPEED = NORMAL_SPEED;
    RIGHT_SPEED = NORMAL_SPEED;
  }

  motors.setSpeeds(LEFT_SPEED, RIGHT_SPEED);
  //delay(100);
}
