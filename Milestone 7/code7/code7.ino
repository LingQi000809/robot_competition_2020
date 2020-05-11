#include <ZumoShield.h>

ZumoMotors motors;
ZumoReflectanceSensorArray sensors;
Pushbutton button(ZUMO_BUTTON);
ZumoBuzzer buzzer;

unsigned int sensorValues[6];
unsigned int lastSensorValues[6];
int sensorChange[6];
int linePos; // position of the line (readLine function)
int error;

// variable
int NORMAL_SPEED = 100; // speed: 0 (stop) - 400 (full)
int MAX_SPEED = 150;
int ADJUSTING_SPEED = 120;
int moveForwardDelayTime = 120;
int turnDelayTime = 200;

int ERROR_THRESHOLD = 500;
int BLACK_THRESHOLD = 300; // > -> blackish
int WHITE_THRESHOLD = 100; // < -> whiteish

int v = NORMAL_SPEED; // CURRENT SPEED
char branches[100]; //l-left; r-right; f-forward
int branchCursor = 0;
int nPath;


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
    Serial.println("----------T----------");
    printValues();
    
    moveForward();
    if (allBlack())
      solved();
    else {
      backToBranch();
      turnRight();
    }
  }

  else if (detectR()) {
    Serial.println("----------R----------");
    printValues();
    turnRight();
  }
  else if (detectL()) {
    Serial.println("----------L----------");
    printValues();
    turnLeft();
  }

  else if (deadEnd()) {
    buzzer.play("cde");
    Serial.println("----------DEADEND----------");
    printValues();
    turnRound();

    while (true) {
      updateReadings();

      if (detectT()) {
        buzzer.play("O5 f");
        Serial.println("----------T----------");
        printValues();
        moveForward();
        if (allBlack())
          solved();
        else {
          backToBranch();
          if (branches[branchCursor - 1] == 'R') {
            turnRight();
            eraseLastPath();
          }
          else if (branches[branchCursor - 1] == 'L') {
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
          buzzer.play("g");
          moveForward();
          eraseLastPath();
        }
        else {
          if (detectL()) {
            Serial.println("----------L----------");
            printValues();
            turnLeft();
          }
          else if (detectR()) {
            Serial.println("----------R----------");
            printValues();
            turnRight();
          }
          for (int i = 0; i < 2; i++) {
            eraseLastPath();
          }
        }
        printBranches();
      }

      else followLine();
      printValues();
    }
  }

  else followLine();
  printValues();
}





void calibration() {
  int CALIBRATION_SPEED = 150;
  for (int t = 0; t <= 80; t++) {
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
  for (int i = 0; i < 6; i++)
    lastSensorValues[i] = sensorValues[i];

  linePos = sensors.readLine(sensorValues); // 0 - directly below sensor 0; 1 - directly below sensor 1; 5 - 5000
  error = linePos - 2500;
  v = abs(error) / LINEPOS_SPEED_RATIO + NORMAL_SPEED;
  sensors.readCalibrated(sensorValues);

  for (int i = 0; i < 6; i++)
    sensorChange[i] = sensorValues[i] - lastSensorValues[i];
}


void eraseLastPath() {
  branchCursor--;
  branches[branchCursor] = ' ';
}




// solving maze
void solved() {
  motors.setSpeeds(0, 0);
  resetValues();
  
  // examine & end
  printBranches();
  buzzerPath();
  while (buzzer.isPlaying());
  
  // launch reduced run
  button.waitForButton();
  buzzer.play("L16 cdegreg4");
  while (buzzer.isPlaying());
  solveMaze();
}


void resetValues(){
  nPath = branchCursor-1;
  branchCursor = 0;
  NORMAL_SPEED = 200;
  MAX_SPEED = 250;
  LINEPOS_SPEED_RATIO = (MAX_LINEPOS - MIDDLE_LINEPOS) / (MAX_SPEED - NORMAL_SPEED);
  ADJUSTING_SPEED = 200;
  moveForwardDelayTime = 50;
  turnDelayTime = 150;
  ERROR_THRESHOLD = 1000;
}
  
void solveMaze(){
  
  while (branchCursor<=nPath){
    updateReadings();
    if (detectT() || detectL() || detectR()){
      action(branchCursor);
    }
    else followLine();
  }
  while (!detectT()){
    updateReadings();
    followLine();
  }
  motors.setSpeeds(0,0);
  buzzer.play("!T250 O4 L8 g O5 ceg O6 ceg4.e4. O4 a- O5 ce-a- O6 ce-a-4.e-4. O4 b- O5 dfb- O6 df b-4r MS b-b-b- ML O7 c2.");
  while (buzzer.isPlaying());
  exit(0);
}

void action(int bCursor){
  char actionType = branches[bCursor];
  if (actionType == 'L') turnLeft();
  else if (actionType == 'R') turnRight();
  else if (actionType == 'F') {
    buzzer.play("O4 g");
    moveForward();
    branchCursor++;
  }
}


// detect
boolean allBlack() {
  return ((sensorValues[0] > BLACK_THRESHOLD)
          && (sensorValues[1] > BLACK_THRESHOLD)
          && (sensorValues[2] > BLACK_THRESHOLD)
          && (sensorValues[3] > BLACK_THRESHOLD)
          && (sensorValues[4] > BLACK_THRESHOLD)
          && (sensorValues[5] > BLACK_THRESHOLD));
}

boolean detectT() {
  return ((sensorChange[1] > 100) && (sensorChange[4] > 100));
}

boolean deadEnd() {
  return ((sensorValues[0] < WHITE_THRESHOLD)
          && (sensorValues[1] < WHITE_THRESHOLD)
          && (sensorValues[2] < WHITE_THRESHOLD)
          && (sensorValues[3] < WHITE_THRESHOLD)
          && (sensorValues[4] < WHITE_THRESHOLD)
          && (sensorValues[5] < WHITE_THRESHOLD));
}

boolean detectL() {
  return ((sensorValues[0] > BLACK_THRESHOLD)
          && (sensorValues[1] > BLACK_THRESHOLD)
          && (sensorValues[2] > BLACK_THRESHOLD));
}

boolean detectR() {
  return ((sensorValues[3] > BLACK_THRESHOLD)
          && (sensorValues[4] > BLACK_THRESHOLD)
          && (sensorValues[5] > BLACK_THRESHOLD));
}



// action
void moveForward() {
  motors.setSpeeds(ADJUSTING_SPEED, ADJUSTING_SPEED);
  delay(moveForwardDelayTime);
  updateReadings();
}

void backToBranch() {
  do {
    motors.setSpeeds(-100, -100);
    updateReadings();
  } while (!allBlack);
  delay(150); // cancel out the moveforward in the turning right function
  updateReadings();
}

void turnRound() {
  moveForward();
  motors.setSpeeds(MAX_SPEED, -MAX_SPEED);
  while (abs(error) > ERROR_THRESHOLD) {
    updateReadings();
  }
  Serial.println("--------FINISHED TURNROUND-----------");
  printValues();
}

void turnRight() {
  buzzer.play("O5 c");
  moveForward();
  motors.setSpeeds(MAX_SPEED, -MAX_SPEED);
  delay(turnDelayTime);
  do {
    updateReadings();
  } while ((abs(error) > ERROR_THRESHOLD) ||
           ((sensorValues[0] > BLACK_THRESHOLD)
            || (sensorValues[1] > BLACK_THRESHOLD)
            || (sensorValues[4] > BLACK_THRESHOLD)
            || (sensorValues[5] > BLACK_THRESHOLD)));
  updateReadings();
  branches[branchCursor] = 'R';
  branchCursor++;
  printBranches();
}

void turnLeft() {
  buzzer.play("O4 c");
  moveForward();
  motors.setSpeeds(-MAX_SPEED, MAX_SPEED);
  delay(turnDelayTime);
  do {
    updateReadings();
  } while ((abs(error) > ERROR_THRESHOLD) ||
           ((sensorValues[0] > BLACK_THRESHOLD)
            || (sensorValues[1] > BLACK_THRESHOLD)
            || (sensorValues[4] > BLACK_THRESHOLD)
            || (sensorValues[5] > BLACK_THRESHOLD)));
  updateReadings();
  branches[branchCursor] = 'L';
  branchCursor++;
  printBranches();
}

void followLine() {
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
  delay(1000);
  for (int i = 0; i < branchCursor; i++) {
    char curPath = branches[i];
    if (curPath == 'L') buzzer.play("O4 c");
    else if (curPath == 'R') buzzer.play("O5 c");
    else if (curPath == 'F') buzzer.play("O4 g");
    delay(1000);
  }
}
void printBranches() {
  Serial.print("[");
  for (int i = 0; i < branchCursor; i++)
    Serial.print(branches[i]);
  Serial.println("]");
  Serial.println();
}

void printValues() {
  for (int i = 0; i < 6; i++) {
    Serial.print(i);
    Serial.print(": ");
    Serial.print(sensorValues[i]);
    Serial.println();
  }
  Serial.println();
}
