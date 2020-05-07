#include <ZumoShield.h>

ZumoMotors motors;
ZumoReflectanceSensorArray sensors;
Pushbutton button(ZUMO_BUTTON);
ZumoBuzzer buzzer;

unsigned int sensorValues[6];
int linePos; // position of the line (readLine function)
int error;

int NORMAL_SPEED = 150; // speed: 0 (stop) - 400 (full)
int MAX_SPEED = 200;
int MAX_LINEPOS = 5000;
int MIDDLE_LINEPOS = MAX_LINEPOS / 2;

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





int v = NORMAL_SPEED; // CURRENT SPEED
char branches[50]; //l-left; r-right; f-forward; t-tShape
// int path[];
int branchCursor = 0;




void loop() {

  updateReadings();



  // T
  if (detectT()) {
    buzzer.play("f");
    motors.setSpeeds(MAX_SPEED / 2, MAX_SPEED / 2);
    delay(200);//while (buzzer.isPlaying());
    if (detectT()) { // end
      motors.setSpeeds(0, 0);
      buzzerPath();
      while (buzzer.isPlaying());
      exit(0);
    }
    else {
      branch();
      printBranches();
    }
  }

  // turning
  else if (( (sensorValues[3] == 1000) && (sensorValues[4] == 1000) && (sensorValues[5] == 1000)) || ((sensorValues[0] == 1000) && (sensorValues[1] == 1000) && (sensorValues[2] == 1000))) {
    turn();
    printBranches();
  }

  // dead-end
  else if (abs(error) == 2500) {
    buzzer.play("cde");
    motors.setSpeeds(0, 0);
    while (abs(sensors.readLine(sensorValues) - 2500) > 500)
      turnRight(); // turn round
    while (true) {
      updateReadings();
      // T
      if (detectT()) {
        buzzer.play("f");
        motors.setSpeeds(MAX_SPEED / 2, MAX_SPEED / 2);
        delay(200);
        if (detectT()) { // end
          motors.setSpeeds(0, 0);
          buzzerPath();
          while (buzzer.isPlaying());
          exit(0);
        }
        else {
          do {
            updateReadings();
            motors.setSpeeds(-MAX_SPEED / 2, -MAX_SPEED / 2);
          } while (abs(error) == 2500);
          if (branches[branchCursor - 1] == 'R') turnRight();
          else if (branches[branchCursor - 1] == 'L') turnLeft();
          branches[branchCursor - 1] = 'F';
          printBranches();
          motors.setSpeeds(0, 0);
          updateReadings();
          followLine();
        }
        buzzer.play("edc");
        break;
      }

      // TURNING
      else if (( (sensorValues[3] == 1000) && (sensorValues[4] == 1000) && (sensorValues[5] == 1000)) || ((sensorValues[0] == 1000) && (sensorValues[1] == 1000)  && (sensorValues[2] == 1000))) {
        if (branches[branchCursor - 1] == 'F') {
          motors.setSpeeds(v, v);
          delay(150); // to avoid follow line and turn
          branchCursor--;
          branches[branchCursor] = ' ';
        }
        else {
          turn();
          branchCursor--;
          branches[branchCursor] = ' '; // cancel the current turn
          branchCursor--;
          branches[branchCursor] = ' '; // cancel the last turn in the array
        }
        printBranches();
      }

      else followLine();
    }
  }

  else followLine();
}




void buzzerPath() {
  //  delay(1000);
  for (int i = 0; i < branchCursor; i++) {
    char curPath = branches[i];
    if (curPath == 'L') buzzer.play("O4 c");
    else if (curPath == 'R') buzzer.play("O5 c");
    else if (curPath == 'F') buzzer.play("O4 g");
  }
}

void branch() {// branch - always turn right
  //buzzer.play("edc");
  do {
    updateReadings();
    motors.setSpeeds(-MAX_SPEED / 2, -MAX_SPEED / 2);
  } while (abs(error) == 2500);
  turnRight();
  branches[branchCursor] = 'R';
  branchCursor++;
  motors.setSpeeds(0, 0);
  updateReadings();
  followLine();
}

boolean deadEnd() {
  sensors.readCalibrated(sensorValues);
  //boolean noneBlack = true;
  for (int i = 0; i < 6; i++) {
    if (sensorValues[i] == 1000) {
      return false;
      //break;
    }
  }
  return true;
}

boolean detectT() {
  sensors.readCalibrated(sensorValues);
  for (int i = 0; i < 6; i++) {
    if (sensorValues[i] != 1000) {
      return false;
      //break;
    }
  }
  return true;
}

void turn() {
  if ( (sensorValues[3] == 1000) && (sensorValues[4] == 1000) && (sensorValues[5] == 1000)) { // turn right
    //motors.setSpeeds(0, 0);
    turnRight();
    branches[branchCursor] = 'R';
    branchCursor++;
  }
  else if ((sensorValues[0] == 1000) && (sensorValues[1] == 1000) && (sensorValues[2] == 1000)) { // turn left
    //motors.setSpeeds(0, 0);
    turnLeft();
    branches[branchCursor] = 'L';
    branchCursor++;
  }
  motors.setSpeeds(0, 0);
  updateReadings();
  followLine();
}

void turnRight() {
  // buzzer.play("O5 c");
  motors.setSpeeds(100, 100);
  delay(200);
  updateReadings();
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
  motors.setSpeeds(100, 100);
  delay(200);
  updateReadings();
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
  //  Serial.print("error = ");
  //  Serial.println(error);
  //  Serial.print("; v = ");
  //  Serial.println(v);
  v = error / LINEPOS_SPEED_RATIO + NORMAL_SPEED;
  if (abs(error) > ERROR_THRESHOLD) {
    if (linePos > 2500)  // left, anti-clockwise
      motors.setSpeeds(v, -v);
    else if (linePos < 2500)  // right, clockwise
      motors.setSpeeds(-v, v);
  }
  else {
    motors.setSpeeds(v, v);
  }
}

void updateReadings() {
  linePos = sensors.readLine(sensorValues); // 0 - directly below sensor 0; 1 - directly below sensor 1; 5 - 5000
  error = linePos - 2500;
  v = error / LINEPOS_SPEED_RATIO + NORMAL_SPEED;
  sensors.readCalibrated(sensorValues);
}


void printBranches() {
  for (int i = 0; i < branchCursor; i++)
    Serial.print(branches[i]);
  Serial.println();
}
