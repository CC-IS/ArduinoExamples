// Blinky example for the esp32-s2 mini
// featuring Serial output aswell!

#include <Arduino.h>


// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
  Serial.begin();
  while(!Serial){;} // wait for serial connection to start! can also use delay(500) to give serial time to start
  pinMode(LED_BUILTIN,OUTPUT); // macro for boards with built in LEDs
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  Serial.println("Hello World!");
  digitalWrite(LED_BUILTIN,1);
  delay(1000);
  digitalWrite(LED_BUILTIN,0);
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}