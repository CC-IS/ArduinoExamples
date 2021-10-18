#include "arduino.h"

static int tonePin = 0;

class stepper {
public:
  int pulsePin;
  int dirPin;
  int enbPin;

  // stores the direction and pulse speed
  int dir;
  int pps;
  float maxPPS;
  float curSpeed;
  float targSpeed;
  float startSpeed;

  unsigned long startTime;
  unsigned long refreshTime;
  int rampTime;

  //Tone pulser;

  bool running;
  bool ramping;

  //stores the pointer to the callback function
  void (*stopCB)();

  //declare the actuator object and set defaults.
  stepper(int pwm, int d, int enb, float mxPPS = 3000){
    pulsePin = pwm;
    enbPin = enb;
    dirPin = d;

    maxPPS = mxPPS;
    pps = 0;
    dir = 0;
    curSpeed = 0;
    targSpeed = 0;

    startTime = 0;
    rampTime = 0;
    refreshTime = 0;

    stopCB = 0;
    
    running = false;
    ramping = false;

    pinMode(dirPin,OUTPUT);
    pinMode(enbPin,OUTPUT);
    digitalWrite(dirPin,LOW);
    digitalWrite(enbPin,LOW);
  }

  int direction(){
    return dir;
  }

  void setDirection(int d){
    dir = d;
    digitalWrite(dirPin,dir);
  }

  void setEnable(int e){
    digitalWrite(enbPin,e);
    if(!e && !ramping){
      if(stopCB) stopCB();
      running = false;
    }
  }
  
  void start(){
    //digitalWrite(enbPin,HIGH);
    setEnable(HIGH);
  }
  
  void stop(){
    setEnable(LOW);
  }

  void setPulseSpeed(int sp){
    pps = sp;
    if(tonePin != pulsePin) noTone(tonePin);
    tonePin = pulsePin;
    tone(pulsePin,sp);
  }

  void run(float spd){
    setPulseSpeed(int(abs(spd) * maxPPS));
    setDirection(spd>=0);
    setEnable(spd != 0);
    running = (spd != 0);
    curSpeed = spd;
  }

  void ramp(float spd, int time){
    if(targSpeed != spd){
      ramping = true;
      startTime = millis();
      rampTime = time;
      targSpeed = spd;
      startSpeed = curSpeed;
    }
  }

  void stopRamp(){
    ramping = false;
  }

  void idle(){
    if(ramping){
      if(startTime + rampTime > millis()){
        float speedOffset = targSpeed - startSpeed;
        float newSpeed = startSpeed + speedOffset * (float(millis()-startTime)/float(rampTime)); 
        run(newSpeed);
      } else {
        ramping = false;
        run(targSpeed);
      }
    }
  }

};
