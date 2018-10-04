#include <Arduino.h>

#define MOTOR_EN_A 11
#define MOTOR_IN1 9
#define MOTOR_IN2 8
#define MOTOR_IN3 7
#define MOTOR_IN4 6
#define MOTOR_EN_B 10

#define READ_NEWLINE_NOW 0
#define READ_DEVICE_NOW 1
#define READ_N_NOW 2

#define LED_INSTANCE 0
#define MOTOR_INSTANCE 1
#define NUM_INSTANCES 2

typedef int instance_type;

instance_type instances[NUM_INSTANCES] = { LED_INSTANCE, MOTOR_INSTANCE };

// protocol handling
String paramBuffer = "";
String nodeName = "dcarm";

typedef struct {
  int stage;
} parse_state;

parse_state parserState = { READ_NEWLINE_NOW };

// state & timing

unsigned long defaultDuration = 500;

typedef struct {
  bool on;
  unsigned long changedAt;
} led_state;

led_state currentState = { false, millis() };
led_state targetState = { false, millis() };

/* e.g. for a attached motor */
typedef struct {
  float speed;
  bool forward;
  unsigned long changedAt;
} motor_state;

motor_state currentMotorState = { 0.0, true, millis() };
motor_state targetMotorState = { 0.0, true, millis() };



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
  
  digitalWrite(2, LOW);  
  Serial.begin(9600);
}

void loop() {
  unsigned long now = millis();
  if (targetState.on != currentState.on) {
    digitalWrite(13, targetState.on ? HIGH : LOW);
    currentState.on = targetState.on;
    currentState.changedAt = millis();
  }
  // lights off after default duration
  if (currentState.on && ((now - currentState.changedAt) >= defaultDuration)) {
    targetState.on = false;
  }

  if (targetMotorState.speed != currentMotorState.speed) {
    currentMotorState.speed = targetMotorState.speed;
    currentMotorState.changedAt = millis();
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, HIGH);
    analogWrite(MOTOR_EN_A, floor(currentMotorState.speed * 255));
  }

  if ((currentMotorState.speed > 0) && ((now - currentMotorState.changedAt) >= defaultDuration)) {
    digitalWrite(MOTOR_IN1, LOW);
    digitalWrite(MOTOR_IN2, LOW);
    analogWrite(MOTOR_EN_A, 0);
    targetMotorState.speed = 0.0;
  }
}

void serialEvent() {
  while(Serial.available() > 0) {
    char currentChar = (char)Serial.read();
    switch (parserState.stage) {
    case READ_DEVICE_NOW:
      if ((currentChar == ' ') && (paramBuffer == nodeName)) {
        parserState.stage = READ_N_NOW;
        paramBuffer = "";
      }
      else {
        // buffering
        paramBuffer += currentChar;
      }
      break;
    case READ_NEWLINE_NOW:
      if (currentChar == '\n') {
        // full message received
        parserState.stage = READ_DEVICE_NOW;
        paramBuffer = "";
      }
      break;
    case READ_N_NOW:
      if (currentChar == ' ') {
        if (paramBuffer.toInt() == 0) {
          targetMotorState.speed = 1.0;
        }
        else {
          targetState.on = true;              
        }
        parserState.stage = READ_NEWLINE_NOW;
      }
      else {
        // buffering
        paramBuffer += currentChar;
      }
      break;
    }
  }
}
