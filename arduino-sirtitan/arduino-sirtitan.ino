/*
   SirTitan
   https://github.com/rafaelalmeidatk
*/

//--------------------------------------------------
// Motors

int motorsE[2] = { 5, 6 };
int motorsM[2] = { 4, 7 };

//--------------------------------------------------
// Moves

const int M_LEFT = 0;
const int M_RIGHT = 1;

const int FRONT = 0;
const int BACK = 1;
const int LEFT = 2;
const int RIGHT = 3;

const int FAST = 0;
const int MEDIUM = 1;
const int SLOW = 2;

int speeds[3] = { 255, 100, 50 };

//--------------------------------------------------
// Current Speed

int currentSpeed = 0;

//--------------------------------------------------
// Movement changed

bool changed = false;

//--------------------------------------------------

void updateMotorsSpeed(int speedId) {
  if (speedId < 0) {
    speedId = currentSpeed;
  }
  for (int i = 0; i < 2; i++) {
    analogWrite(motorsE[i], speeds[speedId]);
  }
}

void motorDirection(int moveId, bool updateSpeed) {
  int e[2];
  switch (moveId) {
    case FRONT:
      e[0] = LOW;
      e[1] = HIGH;
      break;
    case BACK:
      e[0] = HIGH;
      e[1] = LOW;
      break;
    case LEFT:
      e[0] = LOW;
      e[1] = LOW;
      break;
    case RIGHT:
      e[0] = HIGH;
      e[1] = HIGH;
      break;
  }
  for (int i = 0; i < 2; i++) {
    digitalWrite(motorsM[i], e[i]);
  }
  if (updateSpeed) {
    updateMotorsSpeed(-1);
  }
}

void motorSpeed(int speedId, bool updateSpeed, bool keepSpeed) {
  if (keepSpeed) {
    currentSpeed = speedId;
  }
  if (updateSpeed) {
    updateMotorsSpeed(speedId);
  }
}

void moveRobot(int moveId) {
  if (moveId == LEFT || moveId == RIGHT) {
    motorDirection(moveId, false);
    motorSpeed(FAST, true, false);
  } else {
    motorDirection(moveId, true);
  }
}

void serialFlush() {
  while (Serial.available() > 0) {
    char t = Serial.read();
  }
}

void setup()
{
  Serial.begin(9600);
  for (int i = 0; i < 2; i++) {
    pinMode(motorsM[i], OUTPUT);
  }
  digitalWrite(motorsM[0], LOW);
  digitalWrite(motorsM[1], HIGH);
  motorSpeed(SLOW, false, true);
}

void loop()
{
  if (Serial.available() > 0) {
    int incomingByte = Serial.read();
    if (incomingByte == 'W') {
      moveRobot(FRONT);
      changed = true;
    }
    if (incomingByte == 'S') {
      moveRobot(BACK);
      changed = true;
    }
    if (incomingByte == 'A') {
      moveRobot(LEFT);
      changed = true;
    }
    if (incomingByte == 'D') {
      moveRobot(RIGHT);
      changed = true;
    }
    if (incomingByte == '1') {
      motorSpeed(FAST, true, true);
      changed = true;
    }
    if (incomingByte == '2') {
      motorSpeed(MEDIUM, true, true);
      changed = true;
    }
    if (incomingByte == '3') {
      motorSpeed(SLOW, true, true);
      changed = true;
    }
    serialFlush();
    delay(100);
    return;
  }

  if (changed) {
    for (int i = 0; i < 2; i++) {
      analogWrite(motorsE[i], 0);
    }
    changed = false;
  }
}
