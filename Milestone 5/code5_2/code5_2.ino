#include <ZumoShield.h>

ZumoMotors motors;
ZumoReflectanceSensorArray sensors;
Pushbutton button(ZUMO_BUTTON);
ZumoBuzzer buzzer;

unsigned int sensorValues[6];
int linePos; // position of the line (readLine function)
int error;

// variable
int NORMAL_SPEED = 150; // speed: 0 (stop) - 400 (full)
int MAX_SPEED = 200;
int ERROR_THRESHOLD = 500;
int BLACK_THRESHOLD = 700; // > -> blackish
int WHITE_THRESHOLD = 300; // < -> whiteish
int v = NORMAL_SPEED; // CURRENT SPEED
char branches[50]; //l-left; r-right; f-forward
int branchCursor = 0;


int MAX_LINEPOS = 5000;
int MIDDLE_LINEPOS = MAX_LINEPOS / 2;
int LINEPOS_SPEED_RATIO = (MAX_LINEPOS - MIDDLE_LINEPOS) / (MAX_SPEED - NORMAL_SPEED);


void setup() {
  // initialization
  Serial.begin(9600);
  sensors.init();
  delay(500);

  // calibration
  calibration();
  buzzer.play(">g32>>c32");
  motors.setSpeeds(0, 0);

  // launch
  button.waitForButton();
  buzzer.play("L16 cdegreg4");
  while (buzzer.isPlaying());
}

void loop() {
  updateReadings();

  if (detectT()) {
    buzzer.play("O4 f");
    moveForward();
    if (detectT())
      stopRobot();
    else {
      backToBranch();
      turnRight();
    }
  }

  else if (detectR()) {
    turnRight();
  }
  else if (detectL()) {
    turnLeft();
  }

  else if (deadEnd()) {
    buzzer.play("cde");
    turnRound();

    while (true) {
      updateReadings();

      if (detectT()) {
        buzzer.play("O5 f");
        moveForward();
        if (detectT())
          stopRobot();
        else {
          backToBranch();
          // PROBLEMS: F5, F4, C
          if (branches[branchCursor - 1] == 'R') {
            buzzer.play("O5 c");
            turnRight();
            eraseLastPath();
          }
          else if (branches[branchCursor - 1] == 'L') {
            buzzer.play("O4 c");
            turnLeft();
            eraseLastPath();
          }
          branches[branchCursor - 1] = 'F';
        }
        printBranches();
        buzzer.play("edc");
        break;
      }

      else if (detectL() || detectR()) {
        if (branches[branchCursor - 1] == 'F') {
          moveForward();
          eraseLastPath();
        }
        else {
          if (detectL()) turnLeft();
          else if (detectR()) turnRight();
          for (int i = 0; i < 2; i++) {
            eraseLastPath();
          }
        }
        printBranches();
      }

      else followLine();
    }
  }

  else followLine();
}





void calibration() {
  int CALIBRATION_SPEED = 100;
  for (int t = 0; t < 80; t++) {
    // 0-10s turn clockwise; 10-30s turn anti-clockwise; 30-50s turn clockwise; 50-70s turn anti-clockwise; 70-80 turn clockwise back to front
    if ((t <= 10) || (t > 30 && t <= 50) || (t > 70))
      motors.setSpeeds(CALIBRATION_SPEED, -CALIBRATION_SPEED);
    else
      motors.setSpeeds(-CALIBRATION_SPEED, CALIBRATION_SPEED);
    sensors.calibrate();
    delay(20);
  }
}

void updateReadings() {
  linePos = sensors.readLine(sensorValues); // 0 - directly below sensor 0; 1 - directly below sensor 1; 5 - 5000
  error = linePos - 2500;
  v = error / LINEPOS_SPEED_RATIO + NORMAL_SPEED;
  sensors.readCalibrated(sensorValues);
}

void stopRobot() {
  motors.setSpeeds(0, 0);
  buzzerPath();
  while (buzzer.isPlaying());
  exit(0);
}

void eraseLastPath() {
  branchCursor--;
  branches[branchCursor] = ' ';
}



// detect
//boolean detectT() {
//  return ((sensorValues[0] == 1000)
//          && (sensorValues[1] == 1000)
//          && (sensorValues[2] == 1000)
//          && (sensorValues[3] == 1000)
//          && (sensorValues[4] == 1000)
//          && (sensorValues[5] == 1000));
//}

boolean detectT() {
  sensors.readCalibrated(sensorValues);
  for (int i = 0; i < 6; i++) {
    if (sensorValues[i] != 1000){
      return false;
    }
  }
  return true;
}

boolean deadEnd() {
  return ((sensorValues[0] < 300)
          && (sensorValues[1] < 300)
          && (sensorValues[2] < 300)
          && (sensorValues[3] < 300)
          && (sensorValues[4] < 300)
          && (sensorValues[5] < 300));
}

boolean detectL() {
  return ((sensorValues[0] > 700)
          && (sensorValues[1] > 700)
          && (sensorValues[2] > 700));
}

boolean detectR() {
  return ((sensorValues[3] > 700)
          && (sensorValues[4] > 700)
          && (sensorValues[5] > 700));
}



// action
void moveForward() {
  motors.setSpeeds(100, 100);
  delay(200);
  updateReadings();
}

void backToBranch() { // might have problem with cross road
  do {
    motors.setSpeeds(-MAX_SPEED / 2, -MAX_SPEED / 2);
    updateReadings();
  } while (!detectT());
  delay(50); // cancel out the moveforward in the turning right function
}

void turnRound() {
  //  for (int i = 0; i < 3; i++)
  //    moveForward();
  motors.setSpeeds(MAX_SPEED, -MAX_SPEED);
  while (abs(error) > ERROR_THRESHOLD) {
    updateReadings();
  }
}

void turnRight() {
  // buzzer.play("O5 c");
  moveForward();
  motors.setSpeeds(MAX_SPEED, -MAX_SPEED);
  delay(200);
  do {
    updateReadings();
  } while ((abs(error) > ERROR_THRESHOLD) ||
           ((sensorValues[0] > 700)
            || (sensorValues[1] > 700)
            || (sensorValues[4] > 700)
            || (sensorValues[5] > 700)));
  updateReadings();
  branches[branchCursor] = 'R';
  branchCursor++;
  printBranches();
}

void turnLeft() {
  // buzzer.play("O4 c");
  moveForward();
  motors.setSpeeds(-MAX_SPEED, MAX_SPEED);
  delay(200);
  do {
    updateReadings();
  } while ((abs(error) > ERROR_THRESHOLD) ||
           ((sensorValues[0] > 700)
            || (sensorValues[1] > 700)
            || (sensorValues[4] > 700)
            || (sensorValues[5] > 700)));
  updateReadings();
  branches[branchCursor] = 'L';
  branchCursor++;
  printBranches();
}

void followLine() {
  //    Serial.print("linePos = ");
  //    Serial.print(linePos);
  //    Serial.print("error = ");
  //    Serial.print(error);
  //    Serial.print("; v = ");
  //    Serial.println(v);
  if (abs(error) > ERROR_THRESHOLD) {
    if (linePos > 2500)  // left, anti-clockwise
      motors.setSpeeds(v, -v);
    else if (linePos < 2500)  // right, clockwise
      motors.setSpeeds(-v, v);
  }
  else
    motors.setSpeeds(v, v);
}




// examine / debug

void buzzerPath() {
  for (int i = 0; i < branchCursor; i++) {
    char curPath = branches[i];
    if (curPath == 'L') buzzer.play("O4 c");
    else if (curPath == 'R') buzzer.play("O5 c");
    else if (curPath == 'F') buzzer.play("O4 g");
  }
}
void printBranches() {
  for (int i = 0; i < branchCursor; i++)
    Serial.print(branches[i]);
  Serial.println();
}
