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

// protocol handling
String paramBuffer = "";
String nodeName = "dcarm";

Servo servo;

typedef struct {
  int stage;
} parse_state;

parse_state parserState = { READ_NEWLINE_NOW };

// state & timing

unsigned long defaultDuration = 500;
unsigned long currentDuration = defaultDuration;

typedef struct {
  float speed;
  int n;
  unsigned long changedAt;
} actuator_shape;

actuator_shape currentState = { 0.0, 0, millis() };
actuator_shape targetState = { 0.0, 1, millis() };




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

  servo.attach(SERVO_OUT);
  
  digitalWrite(2, LOW);  
  Serial.begin(9600);
}

void nextStage() {
  parserState.stage = (parserState.stage + 1) % NUM_STAGES;
  paramBuffer = "";
}

void setState(int state) {
  parserState.stage = 0;
  paramBuffer = "";
}

void loop() {
  unsigned long now = millis();

  if (targetState.speed != currentState.speed) {
    currentState.speed = targetState.speed;
    currentState.changedAt = millis();
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, HIGH);
    analogWrite(MOTOR_EN_A, floor(currentState.speed * 255));
  }

  if ((currentState.speed > 0) && ((now - currentState.changedAt) >= currentDuration)) {
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, LOW);
    analogWrite(MOTOR_EN_B, 0);
    targetState.speed = 0.0;
  }

  if (targetState.n != currentState.n) {
    currentState.n = targetState.n;
    currentState.changedAt = millis();
    servo.write(abs(currentState.n) % 180);
  }

  if ((currentState.n > 0) && ((now - currentState.changedAt) >= currentDuration)) {
    targetState.n = 0;
  }    
}

void serialEvent() {
  while(Serial.available() > 0) {
    char currentChar = (char)Serial.read();
    switch (parserState.stage) {
    case READ_DEVICE_NOW:
      if ((currentChar == ' ') && (paramBuffer == nodeName)) {
        nextStage();
      }
      else {
        // buffering
        paramBuffer += currentChar;
      }
      break;
    case READ_NEWLINE_NOW:
      if (currentChar == '\n') {
        // full message received
        nextStage();
      }
      break;
    case READ_DUR_NOW:
      if (currentChar == ' ') {
        currentDuration = floor(paramBuffer.toFloat() * 1000);
        nextStage();
      }
      else {
        // buffering
        paramBuffer += currentChar;
      }         
      break;
    case READ_SPEED_NOW:
      if (currentChar == ' ') {
        targetState.speed = paramBuffer.toFloat();
        nextStage();
      }
      else {
        // buffering
        paramBuffer += currentChar;
      }
      break;
    case READ_N_NOW:
      if (currentChar == ' ') {
        targetState.n = paramBuffer.toInt();
        nextStage();
      }
      else {
        // buffering
        paramBuffer += currentChar;
      }
      break;
    }
    
  }
}
