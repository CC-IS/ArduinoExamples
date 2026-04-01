#ifndef GOOG_API
#define GOOG_API

#include <HTTPClient.h>
#include "time.h"
#include "googleJWT.h"
#include "ArduinoJson.h"

// template <typename CT, typename ... A> class gFunction
// : public gFunction<decltype(&CT::operator())()> {};

// class apiFunc{
//   public:
//     virtual void operator()(String);
// };

// template <typename C> class gFunction<C>: public apiFunc {
// private:
//     C mObject;

// public:
//     gFunction(const C & obj) : mObject(obj) {}

//     void operator()(String a) {
//         this->mObject(a);
//     }
// };

enum reqType {
  GET,
  PUT,
  POST
};

class GoogleAPICall {
public:
  unsigned long expiry;
  String token;
  bool debug;
  
  time_t now;
  GoogleAPICall(){
    now = 0;
    expiry = 0;
    debug = false;
  }
  
  bool checkToken(){
    time(&now);
    while(now < 5000){
      vTaskDelay(1000 * portTICK_RATE_MS);
      time(&now);
    }
    return (expiry > now);
  }
  
  void getToken(){
    Serial.println("Getting new token...");
    char* jwt = createJWT();
    if (jwt != nullptr) {
      HTTPClient http;
      http.begin("https://oauth2.googleapis.com/token"); //Specify the URL and certificate
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      String pre = String("grant_type=urn%3Aietf%3Aparams%3Aoauth%3Agrant-type%3Ajwt-bearer&assertion=");
      int httpCode = http.POST(pre+String(jwt));                                                //Make the request
      String res = http.getString();
      http.end(); //Free the resources
      if(debug) Serial.print("Authorizing");
      if (httpCode > 0) { //Check for the returning code
          
          Serial.println(res);
          DynamicJsonDocument doc(2000);                         //Memory pool
          DeserializationError err = deserializeJson(doc,res); //Parse message
         
          if (err) Serial.println("Parsing failed");
          else {
            time(&now);
            token = doc["access_token"].as<String>();
            expiry = now + doc["expires_in"].as<int>();
            Serial.println(doc["expires_in"].as<int>());
            Serial.println("got new access token");
          }
        }
   
      else {
        Serial.print("Error on HTTP request");
        Serial.println(httpCode);
        Serial.println(res);
        getToken();
      }
      free(jwt);
    }
  }

  String handleRequest(String URL, reqType type, String DATA = ""){
    if(!checkToken()) getToken();
    HTTPClient req;
    req.begin(URL);
    req.addHeader("Authorization", String("Bearer ") + token);
    int httpCode = 0;
    if(type == GET) httpCode = req.GET();
    else if(type == PUT) httpCode = req.PUT(DATA);
    else if(type == POST) httpCode = req.POST(DATA);
    String res = req.getString();
    req.end();
    if (httpCode > 0) return res;
    else return handleRequest(URL,type,DATA);
  }

  String get(String URL){
    return handleRequest(URL, GET);
  }

  String post(String URL, String Data){
    return handleRequest(URL, POST, Data);
  }

  String put(String URL, String Data){
    return handleRequest(URL, PUT, Data);
  }
};

GoogleAPICall api;

#endif
