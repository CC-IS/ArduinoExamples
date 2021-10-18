#include "Button.h"

Button example;

void setup() {
  pinMode(13,OUTPUT);

  example.setup(10, [](int state){
    digitalWrite(13,state);
  });

}

void loop() {
  example.idle();
}
