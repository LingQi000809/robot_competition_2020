#include <ZumoShield.h>

ZumoMotors motors;
ZumoReflectanceSensorArray sensors;
Pushbutton button(ZUMO_BUTTON);
ZumoBuzzer buzzer;

unsigned int sensorValues[6];
unsigned int linePos; // position of the line (readLine function)
int error;

int NORMAL_SPEED = 200; // speed: 0 (stop) - 400 (full)
int MAX_SPEED = 300;
int MAX_LINEPOS = 5000;
int MIDDLE_LINEPOS = MAX_LINEPOS / 2;

int LINEPOS_SPEED_RATIO = (MAX_LINEPOS - MIDDLE_LINEPOS) / (MAX_SPEED - NORMAL_SPEED);
int ERROR_THRESHOLD = 1000;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  buzzer.play(">g32>>c32");

  // Initialize the sensors
  sensors.init();

  // calibration by spinning
  delay(1000);
  int CALIBRATION_SPEED = 100;
  for (int t = 0; t < 80; t++) {
    // 0-10s turn clockwise; 10-30s turn anti-clockwise; 30-50s turn clockwise; 50-70s turn anti-clockwise; 70-80 turn clockwise back to front
    if ((t <= 10) || (t > 30 && t <= 50) || (t > 70)) {
      motors.setSpeeds(CALIBRATION_SPEED, -CALIBRATION_SPEED);
    }
    else {
      motors.setSpeeds(-CALIBRATION_SPEED, CALIBRATION_SPEED);
    }
    sensors.calibrate();
    delay(20);
  }

  buzzer.play(">g32>>c32");
  motors.setSpeeds(0, 0);
  // Wait for the user button to be pressed and released
  button.waitForButton();
  buzzer.play("L16 cdegreg4");
  while (buzzer.isPlaying());
}


int v; // CURRENT SPEED
int LEFT_SPEED;
int RIGHT_SPEED;
int lastLinePos = 2500;

// int path[];
// int cursor = 0； // updated to know the next path

void loop() {

  sensors.readCalibrated(sensorValues);

  // end of the maze
  boolean allBlack = true;
  for (int i = 1; i < 6; i++) {
    if (sensorValues[i] != 1000) {
      allBlack = false;
      break;
    }
  }
  if (allBlack) {
    motors.setSpeeds(0, 0);
    exit(0);
  }


  // turning
  if ((sensorValues[4] == 1000) && (sensorValues[5] == 1000)) { // turn right
    buzzer.play("c32"); //store the move!
    while (abs(sensors.readLine(sensorValues) - 2500) > ERROR_THRESHOLD / 2) {
      motors.setSpeeds(MAX_SPEED, -MAX_SPEED);
    }
  }
  else if ((sensorValues[0] == 1000) && (sensorValues[1] == 1000)) { // turn left
    buzzer.play("g32");
    while (abs(sensors.readLine(sensorValues) - 2500) > ERROR_THRESHOLD / 2) {
      motors.setSpeeds(-MAX_SPEED, MAX_SPEED);
    }
  }



  // follow line
  linePos = sensors.readLine(sensorValues); // 0 - directly below sensor 0; 1 - directly below sensor 1; 5 - 5000
  error = linePos - 2500;
  v = error / LINEPOS_SPEED_RATIO + NORMAL_SPEED;
  //  Serial.print("error = ");
  //  Serial.println(error);
  //  Serial.print("; v = ");
  //  Serial.println(v);
  if (abs(error) > ERROR_THRESHOLD) {
    if (linePos > 2500) { // left, anti-clockwise
      motors.setSpeeds(v, 0);
    }
    else if (linePos < 2500) { // right, clockwise
      motors.setSpeeds(0, v);
    }
  }
  else {
    motors.setSpeeds(v, v);
  }



  // reset lastLinePos
  lastLinePos = linePos;
}
