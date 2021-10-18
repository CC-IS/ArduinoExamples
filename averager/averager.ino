#include "averager.h"

averager exampleAve(10);


void setup() {
  
}


void loop() {
  exampleAve.idle(analogRead(0));

  //retrieve average:
  exampleAve.ave;
  
}
