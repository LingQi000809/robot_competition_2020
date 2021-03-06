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
int NORMAL_SPEED = 230; // speed: 0 (stop) - 400 (full)
int MAX_SPEED = 280;
int ADJUSTING_SPEED = 230; 
int moveForwardDelayTime = 50;
int turnDelayTime = 150;
int ERROR_THRESHOLD = 1000;

int BLACK_THRESHOLD = 300; // > -> blackish
int WHITE_THRESHOLD = 100; // < -> whiteish

int v = NORMAL_SPEED; // CURRENT SPEED
char branches[] = {'L', 'F', 'F', 'R', 'L', 'L', 'R', 'R', 'F', 'R', 'R', 'L', 'R', 'L', 'R'};
int branchCursor = 0;
int nPath = 14;


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
  solveMaze();
}





void calibration() {
  int CALIBRATION_SPEED = 140;
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
  // reset
  nPath = branchCursor;
  branchCursor = 0;
  
  // examine & end
  printBranches();
//  buzzer.play("!T250 O4 L8 g O5 ceg O6 ceg4.e4. O4 a- O5 ce-a- O6 ce-a-4.e-4. O4 b- O5 dfb- O6 df b-4r MS b-b-b- ML O7 c2.");
//  while (buzzer.isPlaying());
  buzzerPath();
  while (buzzer.isPlaying());
  
  // launch reduced run
  // button.waitForButton();
  delay(3000);
  buzzer.play("L16 cdegreg4");
  while (buzzer.isPlaying());
  solveMaze();
}

void solveMaze(){
  while (branchCursor<=nPath){
    updateReadings();
    if (detectT() || detectL() || detectR()){
      //buzzer.play("ceg");
      action(branchCursor);
    }
    else followLine();
  }
  //buzzer.play("gfedc");
  while (!detectT()){
    updateReadings();
    followLine();
  }
  motors.setSpeeds(0,0);
  delay(200);
  buzzer.play("!T250 O3 L8 g O4 ceg O5 ceg4.e4. O3 a- O4 ce-a- O5 ce-a-4.e-4. O3 b- O4 dfb- O5 df b-4r MS b-b-b- ML O6 c2.");
  while (buzzer.isPlaying());
  exit(0);
}

void action(int bCursor){
  char actionType = branches[bCursor];
  if (actionType == 'L') {
    buzzer.play("O4 c");
    turnLeft();
  }
  else if (actionType == 'R') {
    buzzer.play("O5 c");
    turnRight();
  }
  else if (actionType == 'F'){
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
  return ((sensorChange[1] > 70) && (sensorChange[4] > 70));
}
//
//boolean detectT() {
//  return ((sensorValues[0] > 100)
//          && (sensorValues[1] > 100)
//          && (sensorValues[2] > 100)
//          && (sensorValues[3] > 100)
//          && (sensorValues[4] > 100)
//          && (sensorValues[5] > 100));
//}

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

void backToBranch() { // might have problem with cross road
  do {
    motors.setSpeeds(-ADJUSTING_SPEED, -ADJUSTING_SPEED);
    updateReadings();
  } while (!allBlack);
  delay(150); // cancel out the moveforward in the turning right function
  updateReadings();
}

void turnRound() {
  //  for (int i = 0; i < 3; i++)
  moveForward();
  motors.setSpeeds(MAX_SPEED, -MAX_SPEED);
  while (abs(error) > ERROR_THRESHOLD) {
    updateReadings();
  }
  Serial.println("--------FINISHED TURNROUND-----------");
  printValues();
}

void turnRight() {
  //buzzer.play("O5 c");
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
  //buzzer.play("O4 c");
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
