#include "arduino.h"

class ValueMonitor {
public:
  float value;
  float lastValue;
  float changeVal;
  float mn;
  float mx;
  float divs;
  void (*changeCB)(float val);
  float (*idleCB)();

  ValueMonitor(){
    mn = 0;
    mx = 0;
    divs = 0;
  }

  void setup(float (*idlecb)(), void (*CB)(float), float percentChange = .05) {
    changeCB = CB;
    idleCB = idlecb;
    value = lastValue = 0;
    if(!divs) changeVal = percentChange;
  }

  void divide(float minVal, float maxVal,float numDivs){
    divs = numDivs;
    mn = minVal;
    mx = maxVal;
    changeVal = (mx-mn)/(numDivs - 1);
  }
  
  void idle(){
    value = idleCB() / changeVal;
    int check = round(value - lastValue);
    if(check && round(value) != round(lastValue)){
      if(!divs) changeCB(round(value) * changeVal);
      else changeCB(round(value));
      lastValue = value;
    }
  }
};
