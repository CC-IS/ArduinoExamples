#include "serialParser.h"

#define DIGI_READ 1
#define ANA_READ 2
#define DIGI_WRITE 4
#define PWM_WRITE 8
#define READY 127

serialParser parser(Serial);

void setup() {
  Serial.begin(115200);
  parser.address = 1;

  parser.on(READY, [](unsigned char * input, int size){
    parser.sendPacket(REPORT,READY);
  });

  parser.on(DIGI_READ, [](unsigned char * input, int size){
    pinMode(input[2],INPUT_PULLUP);
    parser.sendPacket(REPORT,DIGI_READ, input[2], digitalRead(input[2]));
  });

  parser.on(ANA_READ, [](unsigned char * input, int size){
    parser.sendPacket(REPORT,ANA_READ, input[2], analogRead(input[2]));
  });

  parser.on(DIGI_WRITE, [](unsigned char * input, int size){
    pinMode(input[2],OUTPUT);
    digitalWrite(input[2], input[3]);
  });

  parser.on(PWM_WRITE, [](unsigned char * input, int size){
    pinMode(input[2],OUTPUT);
    analogWrite(input[2], (input[3]<<7) + input[4]);
  });

  parser.sendPacket(REPORT,READY);
}

void loop() {
  parser.idle();
}
