#include <Arduino.h>
#include <Servo.h>

// PINS
#define MOTOR_EN_A 11
#define MOTOR_IN1 9
#define MOTOR_IN2 8
#define MOTOR_IN3 7
#define MOTOR_IN4 6
#define MOTOR_EN_B 10
#define SERVO_OUT 12

// STATES
#define NUM_STAGES 5
// NOTE: number defines order!
#define READ_NEWLINE_NOW 0
#define READ_DEVICE_NOW 1
#define READ_DUR_NOW 2
#define READ_N_NOW 3
#define READ_SPEED_NOW 4

String nodeNames[] = { "dcarm", "dcleg" };

// protocol handling
String paramBuffer = "";
String nodeName = nodeNames[NODE_NAME_IDX];
char currentChar;
Servo servo;

typedef struct {
  int stage;
} parse_state;

parse_state parserState = { READ_NEWLINE_NOW };

// state & timing

unsigned long defaultDuration = 500;
unsigned long currentDuration = defaultDuration;

typedef struct {
  bool light; // led for debugging 
  float speed;
  int n;
  unsigned long changedAt;
} actuator_shape;

actuator_shape currentState = { false, 0.0, 0, millis() };
actuator_shape targetState = { false, 0.0, 120, millis() };

void setup() {
  digitalWrite(13, LOW); // LED13 initially off
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

  pinMode(MOTOR_EN_A, OUTPUT);  
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_IN3, OUTPUT);
  pinMode(MOTOR_IN4, OUTPUT);
  pinMode(MOTOR_EN_B, OUTPUT);
  pinMode(SERVO_OUT, OUTPUT);

  servo.attach(SERVO_OUT);

  // set as rs485 receiver
  digitalWrite(2, LOW);
  
  Serial.begin(9600);
}

void nextStage() {
  parserState.stage = (parserState.stage + 1) % NUM_STAGES;
  paramBuffer = "";
}

void setStage(int state) {
  parserState.stage = 0;
  paramBuffer = "";
}

void loop() {
  unsigned long now = millis();

  if (targetState.speed != currentState.speed) {
    currentState.speed = targetState.speed;
    currentState.changedAt = millis();
    if (currentState.speed > 0) {
      digitalWrite(MOTOR_IN1, LOW);
      digitalWrite(MOTOR_IN2, HIGH);
    }
    else {
      digitalWrite(MOTOR_IN1, HIGH);
      digitalWrite(MOTOR_IN2, LOW);
    }
    analogWrite(MOTOR_EN_A, floor(min(1.0,abs(currentState.speed)) * 255));
  }

  if ((currentState.speed != 0.0) && ((now - currentState.changedAt) >= currentDuration)) {
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, LOW);
    analogWrite(MOTOR_EN_A, 0);
    targetState.speed = 0.0;
  }

  if (targetState.n != currentState.n) {
    currentState.n = targetState.n;
    currentState.changedAt = millis();
    currentState.light = targetState.light;
    digitalWrite(13, currentState.light ? HIGH : LOW);

    servo.write(currentState.n);
  }

  if ((currentState.n > 0) && ((now - currentState.changedAt) >= currentDuration)) {
    targetState.n = 90;
    targetState.light = false;
  }

  if (targetState.light != currentState.light) {
  }

  if (currentState.light && ((now - currentState.changedAt) >= currentDuration)) {
  }
}

void serialEvent() {
  while(Serial.available() > 0) {
    char currentChar = (char)Serial.read();
    switch (parserState.stage) {
    case READ_DEVICE_NOW:
      if ((currentChar == ' ') || (currentChar == '\n')) {
        if (paramBuffer == nodeName) {
          nextStage();
        }
        else {
          setStage(READ_NEWLINE_NOW);
        }
      }
      else { paramBuffer += currentChar; }
      break;
    case READ_NEWLINE_NOW:
      if ((currentChar == ' ') || (currentChar == '\n')) {
        // full message received
        nextStage(); // wraps around
      }
      break;
    case READ_DUR_NOW:
      if ((currentChar == ' ') || (currentChar == '\n')) {
        currentDuration = floor(paramBuffer.toFloat() * 1000);
        nextStage();
      }
      else { paramBuffer += currentChar; }         
      break;
    case READ_SPEED_NOW:
      if ((currentChar == ' ') || (currentChar == '\n')) {
        targetState.speed = paramBuffer.toFloat();
        nextStage();
      }
      else { paramBuffer += currentChar; }
      break;
    case READ_N_NOW:
      if ((currentChar == ' ') || (currentChar == '\n')) {
        targetState.n = paramBuffer.toInt();
        targetState.light = true;
        nextStage();        
      }
      else { paramBuffer += currentChar; }
      break;
    }
    // we just read the last param and skip waiting for a newline
    if ((parserState.stage == READ_NEWLINE_NOW) && (currentChar == '\n')) {
      nextStage();
    }
  }
}
