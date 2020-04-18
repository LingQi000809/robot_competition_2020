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
int v; // CURRENT SPEED
int MAX_LINEPOS = 5000;
int MIDDLE_LINEPOS = MAX_LINEPOS / 2;
int LEFT_SPEED;
int RIGHT_SPEED;

int LINEPOS_SPEED_RATIO = (MAX_LINEPOS - MIDDLE_LINEPOS) / (MAX_SPEED - NORMAL_SPEED);
int ERROR_THRESHOLD = 500;

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

void loop() {
  linePos = sensors.readLine(sensorValues);
  // 0 - directly below sensor 0
  // 1 - directly below sensor 1
  // 5 - 5000

  error = linePos - 2500;
  //errorChange = error-lastError;
  v = error / LINEPOS_SPEED_RATIO + NORMAL_SPEED;
  //  Serial.print("error = ");
  //  Serial.print(error);
  //  Serial.print("; v = ");
  //  Serial.println(v);

  if (abs(error) == 2500) {
    motors.setSpeeds(0, 0);
    exit(0);
  }
  else if (abs(error) > ERROR_THRESHOLD) {
    if (linePos > 2500) {
      // left, anti-clockwise
      LEFT_SPEED = v;
      RIGHT_SPEED = 0;
    }
    else if (linePos < 2500) {
      // right, clockwise
      LEFT_SPEED = 0;
      RIGHT_SPEED = v;
    }
  }
  else {
    LEFT_SPEED = v;
    RIGHT_SPEED = v;
  }

  motors.setSpeeds(LEFT_SPEED, RIGHT_SPEED);
}
