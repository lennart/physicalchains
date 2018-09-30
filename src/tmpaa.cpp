
#include <Arduino.h>
#include <Chirp.h>
Chirp chirp;
// the setup function runs once when you press reset or power the board
void setup() {
  // current from the linear regulator (use 3.3V psu)
  //  analogReference(EXTERNAL);
  // initialize digital pin 13 as an output.
  pinMode(2, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);              // wait for a second
  digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);              // wait for a second
  chirp.chirp("parrotbilllllahcm4");
  delay(2000);  
}


