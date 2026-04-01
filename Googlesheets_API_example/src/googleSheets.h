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

  DynamicJsonDocument getCells(char* key, bool printRes = false){
    char query[200];
    snprintf(query, sizeof(query),"https://sheets.googleapis.com/v4/spreadsheets/%s/values/%s!%s", id, sheet, key);
    String output = api.get(query);
    if(printRes) Serial.println(output);
    DynamicJsonDocument doc(2000);
    DeserializationError error = deserializeJson(doc, output);

    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
    }

    return doc;
  }

  DynamicJsonDocument appendCells(char* key, String values, bool printRes = false){
    char query[200];
    snprintf(query, sizeof(query),"https://sheets.googleapis.com/v4/spreadsheets/%s/values/%s!%s:append?valueInputOption=USER_ENTERED", id, sheet, key);
    String body = "{\"range\":\"" + String(sheet) + "!" + String(key) +"\",\"majorDimension\":\"ROWS\","+values+"}";
    Serial.println(body);
    String output = api.post(query, body);
    if(printRes) Serial.println(output);
    DynamicJsonDocument doc(2000);
    DeserializationError error = deserializeJson(doc, output);

    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
    } 
    return doc;
  }

  DynamicJsonDocument putCells(String key, String values, bool printRes = false){
    char query[200];
    snprintf(query, sizeof(query),"https://content-sheets.googleapis.com/v4/spreadsheets/%s/values/%s!%s?valueInputOption=USER_ENTERED", id, sheet, key);
    Serial.println(query);
    String body = "{\"range\":\"" + String(sheet) + "!" + String(key) +"\",\"majorDimension\":\"ROWS\","+values+"}";
    String output = api.put(query, body);
    if(printRes) Serial.println(output);
    DynamicJsonDocument doc(2000);
    DeserializationError error = deserializeJson(doc, output);

    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
    } 
    return doc;
  }
};

#endif