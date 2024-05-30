#include <Arduino.h>
#include <servo.h>

Servo myservo;
const int POTPIN = A0;
const int SERVOPIN = 8;

void setup() {
  Serial.begin(9600);
  pinMode(POTPIN, INPUT);
  myservo.attach(SERVOPIN);
}

void loop() {
  int potValue = analogRead(POTPIN);
  potValue = map(potValue, 0, 1023, 0, 90);
  myservo.write(potValue);
}