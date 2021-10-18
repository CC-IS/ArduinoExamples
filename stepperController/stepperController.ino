#include "stepperControl.h"

stepper motor(10, 6, 7);

bool fired = 0;

int sensor = 11;

void setup() {
  // put your setup code here, to run once:
  pinMode(sensor,INPUT_PULLUP);
  pinMode(13,OUTPUT);
  //motor.run(1);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  motor.idle();

  if(!digitalRead(sensor) && !fired){
    fired = true;
    digitalWrite(13,LOW);
    motor.ramp(0,500);
  } else if(digitalRead(sensor) && fired){
    digitalWrite(13,HIGH);
    fired = false;
    motor.ramp(.5,500);
  }
}
