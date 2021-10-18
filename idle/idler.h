#include "arduino.h"

void idle();
void mainLoop();

void idleDelay(int time){
  unsigned long end = millis() + time;
  while(end > millis()){
    idle();
  }
}

void loop(){
  idle();
  mainLoop();
}

