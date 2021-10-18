#include "timeOut.h"
#include "idler.h"


int blinkTime = 500;

//variable to store which timer is being used
int whichTimeout = 0;

// function to turn light on, and set a timeout to turn it off
void lightOn(){
  digitalWrite(13,HIGH);
  clearTimeout(whichTimeout);
  whichTimeout = setTimeout(lightOff, blinkTime);
}

// function to turn light off, and set a timeout to turn it on
void lightOff(){
  digitalWrite(13,LOW);
  clearTimeout(whichTimeout);
  whichTimeout = setTimeout(lightOn, blinkTime);
}


void setup() {
  //turn on the LED
  pinMode(13,OUTPUT);
  digitalWrite(13,HIGH);

  //You can set timer in one of two ways; passing a function directly, like this:
  whichTimeout = setTimeout([](){
    digitalWrite(13,LOW);
  }, 2000);

  //this allows the timers to run, but pauses program operation otherwise.
  // so, since we set a timer for two seconds, the LED will stay on for 2 seconds, and then stay off
  // for a second, before moving on.
  idleDelay(3000);

  // Or you can set a timer by passing in a named function, like so:
  whichTimeout = setTimeout(lightOn, blinkTime);

  // the light will blink while we idle delay, for 5 seconds.
  idleDelay(5000);

  // after the five seconds elapses, we stop the blink, by canceling the timeout.
  clearTimeout(whichTimeout);


  // and we wait for a second. No timers are running, so nothing happens here.
  idleDelay(1000);

  // change the blink time,
  blinkTime = 250;

  // and restart the blinking, which will now run indefinitely, since we move onto the loop now.
  whichTimeout = setTimeout(lightOn, blinkTime);
}


void mainLoop() {
}

//runs before every mainLoop cycle, and during any idleDelay calls.
void idle(){
  idleTimers();
}

