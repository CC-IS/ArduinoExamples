#ifndef ASYNC_REQ
#define ASYNC_REQ

#include <HTTPClient.h>
#include "time.h"
#include "ArduinoJson.h"

template <typename CT, typename ... A> class jfunction
: public jfunction<decltype(&CT::operator())()> {};

class jsonFunc{
  public:
    virtual void operator()(DynamicJsonDocument&);
};

template <typename C> class jfunction<C>: public jsonFunc {
private:
    C mObject;

public:
    jfunction(const C & obj) : mObject(obj) {}

    void operator()(DynamicJsonDocument& a) {
        this->mObject(a);
    }
};

class JSONCall;

static void makeCall(void * Data);

enum callType{
  GET_JSON,
  POST_JSON,
  PUT_JSON
};

class JSONCall {
public:
  //unsigned long expiry;
  //String token;
  callType type;
  String url;
  String data;
  bool debug;
  time_t now;
  bool lambda;
  bool running;

  void (*voidCB)(DynamicJsonDocument&);
  jsonFunc* callback;

  JSONCall(){
    now = 0;
    //expiry = 0;
    debug = false;
    type = GET_JSON;
    running = false;
  }

  void call(){
    HTTPClient req;
    req.begin(url);
    int httpCode = 0;
    if(type == GET_JSON) httpCode = req.GET();
    else if(type == POST_JSON) httpCode = req.POST(data);
    else if(type == PUT_JSON) httpCode = req.PUT(data);
    if (httpCode > 0) {
      DynamicJsonDocument doc(2000);
      DeserializationError error = deserializeJson(doc, req.getString());
  
      // Test if parsing succeeds.
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
      } else {
        if(!lambda) voidCB(doc);
        else (*callback)(doc);
        delete callback;
      }
    } else if(debug){
      Serial.println("HTTP Error:");
      Serial.println(httpCode);
    }
  }

  template<typename C>
  void request(String URL, const C & CB, String DATA=""){
    if(DATA.length()) type = POST_JSON;
    else type = GET_JSON;
    url = URL;
    callback = new jfunction<C>(CB);
    running = true;
    lambda = true;
    startRequest();
  }

  void request(String URL, void (*CB)(DynamicJsonDocument&), String DATA = ""){
    if(DATA.length()) type = POST_JSON;
    else type = GET_JSON;
    url = URL;
    running = true;
    voidCB = CB;
    startRequest();
  }

  void startRequest(){
    xTaskCreate(
      makeCall,    // Function that should be called
      "Make httpRequest to handle data",   // Name of the task (for debugging)
      100000,            // Stack size (bytes)
      this,            // Parameter to pass
      1,               // Task priority
      NULL             // Task handle
    );
  }
} web;

static void makeCall(void * Data){
  Serial.println("got here");
  ((JSONCall *)Data)->call();
  vTaskDelete(NULL);
}


#endif
