#include <Arduino.h>

int level;
int last_level = 0;

void setup() {
  Serial.begin(9600);
}


void loop() {
  last_level = level;
  level = analogRead(A0);

  if (abs(last_level - level) > 10) {
    Serial.println(level);
    last_level = 0;
  }
}

