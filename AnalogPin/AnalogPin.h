#include "arduino.h"
#include <math.h>

class AnalogPin {
public:
  float value;
  float lastValue;
  float changeVal;
  int pin;
  void (*changeCB)(float val);
  void (*minCB)();
  void (*maxCB)();

  AnalogPin(){
  }

  void setup(int p, void (*CB)(float), float percentChange = .05) {
    changeCB = CB;
    pin = p;
    value = lastValue = 0;
    changeVal = percentChange;
  }
  
  void idle(){
    value = analogRead(pin)/1024.;
    value = value + (changeVal - fmod(value, changeVal));
    if(abs(value - lastValue) > changeVal){
      changeCB(value);
      lastValue = value;
    }
  }
};
