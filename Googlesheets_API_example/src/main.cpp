#include <base64.h>
#include <WiFi.h>
#include "ArduinoJson.h"
#include "AsyncJsonRequest.h"
#include "button.h"
#include "googleSheets.h"

time_t date;

// our target sheet, and sheet "tab"
GoogleSheet store("1BB_ldN-lp5vUgZPTjExYcrW0BpsKIIkrPtvnAG0ifOE","Sheet1");

const char* ssid = "CarletonGuests";//wlan information
const char* pword = "";//wlan information

void putData(void * Data){
  Serial.println("posting data");
  const char* newBuff = "\"values\":[[\"hi\",\"wrld\"]]";
  store.appendCells("A:Z",newBuff,[](DynamicJsonDocument& doc){
    Serial.println("Put Data");
  });
  //vTaskDelete(NULL);
}


void setup()
{
  Serial.begin(115200);
  delay(1000);


  Serial.println("Starting Up...");

  WiFi.begin(ssid,pword);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  time_t now;
  while(now < 1703027354){ //and wait for it to be a unix time later than december 19th, 2023
    time(&now);
  }

  date = now;

}
unsigned long tmr = 0;
unsigned long timeCheck = 0;
static uint8_t hue;
char dataBuf[200];
 
void loop()
{
  
  delay(5000);
  putData(NULL);

}