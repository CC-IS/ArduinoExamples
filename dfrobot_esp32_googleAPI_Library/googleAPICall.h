#ifndef GOOG_API
#define GOOG_API

#include <HTTPClient.h>
#include "time.h"
#include "googleJWT.h"
#include "ArduinoJson.h"

template <typename CT, typename ... A> class function
: public function<decltype(&CT::operator())()> {};

class apiFunc{
  public:
    virtual void operator()(String);
};

template <typename C> class function<C>: public apiFunc {
private:
    C mObject;

public:
    function(const C & obj) : mObject(obj) {}

    void operator()(String a) {
        this->mObject(a);
    }
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
    char* jwt = createJWT();
    if (jwt != nullptr) {
      HTTPClient http;
      
      http.begin("https://oauth2.googleapis.com/token"); //Specify the URL and certificate
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      String pre = String("grant_type=urn%3Aietf%3Aparams%3Aoauth%3Agrant-type%3Ajwt-bearer&assertion=");
      int httpCode = http.POST(pre+String(jwt));                                                  //Make the request

      if(debug) Serial.print("Authorizing");
      if (httpCode > 0) { //Check for the returning code
          String res = http.getString();
   
          DynamicJsonDocument doc(2000);                         //Memory pool
          DeserializationError err = deserializeJson(doc,res); //Parse message
         
          if (err) Serial.println("Parsing failed");
          else {
            time(&now);
            token = doc["access_token"].as<String>();
            expiry = now + doc["expires_in"].as<int>();
            Serial.println("got new access token");
          }
        }
   
      else {
        Serial.print("Error on HTTP request");
        Serial.println(httpCode);
      }
   
      http.end(); //Free the resources
      free(jwt);
    }
  }

  template<typename C>
  void get(String URL, const C & CB){
    if(!checkToken()) getToken();
    HTTPClient req;
    req.begin(URL);
    req.addHeader("Authorization", String("Bearer ") + token);
    int httpCode = req.GET();
    if (httpCode > 0) {
      String res = req.getString();
      CB(res);
    }
  }

  template<typename C>
  void post(String URL, String Data, const C & CB){
    if(!checkToken()) getToken();
    HTTPClient req;
    req.begin(URL);
    req.addHeader("Authorization", String("Bearer ") + token);
    int httpCode = req.POST(Data);
    if (httpCode > 0) {
      String res = req.getString();
      CB(res);
    }
  }
};

GoogleAPICall api;

#endif
