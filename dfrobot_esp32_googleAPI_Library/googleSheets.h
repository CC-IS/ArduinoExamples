#ifndef GOOG_SHEETS
#define GOOG_SHEETS

#include "googleAPICall.h"

class GoogleSheet {
public:
  char* id;
  char* sheet;
  
  GoogleSheet(char* ssid, char* sub){
    id = ssid;
    sheet = sub;
  }

  void getCells(char* key, void (*CB)(DynamicJsonDocument&), bool printRes = false){
    char query[200];
    snprintf(query, sizeof(query),"https://sheets.googleapis.com/v4/spreadsheets/%s/values/%s!%s", id, sheet, key);
    api.get(query, [&](String & output){
      if(printRes) Serial.println(output);
      DynamicJsonDocument doc(2000);
      DeserializationError error = deserializeJson(doc, output);
  
      // Test if parsing succeeds.
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
      } else CB(doc);
    });
  }
};

#endif
