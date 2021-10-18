#define BUTTON_PIN 39

#include <WiFi.h>
#include "ArduinoJson.h"
#include "time.h"
#include "mkrLogo.h"
#include <Wire.h>
#include <SPI.h>
#include <PN532_HSU.h>
#include <PN532.h>
#include "googleSheets.h"
#include <HTTPClient.h>

int authColumn = 2;  // Zero indexed

#define RXD2 19
#define TXD2 22

PN532_HSU pn532hsu( Serial );
PN532 nfc( pn532hsu );

GoogleSheet auths("1k3eZkkqm1bWA3lk8gUgfoR6Xpb2vVaX4iaqnizi5iDc","Authorizations");

class user {
public:
  String id;
  bool auth;
};

user users[1000];
int numUsers = 0;

bool usersReady = false;

const char* ssid = "CarletonGuests"; //wlan information

//https://stackoverflow.com/questions/38857531/how-to-make-post-requests-to-google-spreadsheets-using-javascript/38871487

void getUsers(void * Data){
  auths.getCells("A:Z",[](DynamicJsonDocument& doc){
    Serial.println("Returned doc");
    JsonArray vals = doc["values"].as<JsonArray>();
    numUsers = vals.size() - 1;
    for(int i=0; i < numUsers; i++){
      users[i].id = vals[i+1][0].as<String>();
      users[i].auth = vals[i+1][authColumn].as<int>();
      Serial.println(users[i].id + " is " + users[i].auth);
    }
  });
  usersReady = true;
  vTaskDelete(NULL);
}

void setup()
{

  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");

  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  //https://savjee.be/2020/01/multitasking-esp32-arduino-freertos/
  xTaskCreate(
    getUsers,    // Function that should be called
    "Get Google Access Token",   // Name of the task (for debugging)
    60000,            // Stack size (bytes)
    NULL,            // Parameter to pass
    1,               // Task priority
    NULL             // Task handle
  );
}



void loop()
{
  while(usersReady){
    Serial.println("Ready");
    uint8_t success;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
    uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

    if(success){
      Serial.(String((char *)uid));
    }
  }
}
