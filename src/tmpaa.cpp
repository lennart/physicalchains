#include <Arduino.h>

// protocol handling
String paramBuffer = "";
String nodeName = "dcarm";

#define READ_NEWLINE_NOW 0
#define READ_DEVICE_NOW 1
#define READ_N_NOW 2

#define NUM_INSTANCES 2

typedef struct {
  bool stage;
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
  unsigned long changedAt;
} motor_state;

motor_state currentMotorState = { false, millis() };
motor_state targetMotorState = { false, millis() };

void setup() {
  digitalWrite(13, LOW); // LED13 initially off
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
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
}

void serialEvent() {
  while(Serial.available() > 0) {
    char currentChar = (char)Serial.read();

    if (currentChar == '\n') {
      // full message received
      parserState.stage = READ_DEVICE_NOW;
      paramBuffer = "";
    }
    else if (parserState.stage == READ_DEVICE_NOW) {
      if (currentChar == ' ') {
        if (parserState.stage == READ_N_NOW) {
            // read int
          //paramBuffer.toInt() % NUM_INSTANCES;
          targetState.on = true;
          parserState.stage = READ_NEWLINE_NOW;
        }
        else {
          if (paramBuffer == nodeName) {
            // this message is for us
            parserState.stage = READ_N_NOW;
          }
        }
      }
      else {
        // buffering
        if (parserState.stage != READ_NEWLINE_NOW) {
          paramBuffer += currentChar;
        }
      }
    }
    else {
      // skipping until newline
    }
  }
}
