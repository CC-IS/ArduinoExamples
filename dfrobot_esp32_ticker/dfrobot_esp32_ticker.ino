#include <GxEPD.h>
#include "SD.h"
#include "SPI.h"


#include <WiFi.h>
#include <HTTPClient.h>
#include <esp_wpa2.h> //wpa2 library for connections to Enterprise networks
#include <esp_wifi.h>
#include "ArduinoJson.h"
#include "time.h"
#include "mkrLogo.h"

//MJYCU39KCJ55PEYF

const char* root_ca= \
"-----BEGIN CERTIFICATE-----\n" \
"MIIDSjCCAjKgAwIBAgIQRK+wgNajJ7qJMDmGLvhAazANBgkqhkiG9w0BAQUFADA/\n" \
"MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
"DkRTVCBSb290IENBIFgzMB4XDTAwMDkzMDIxMTIxOVoXDTIxMDkzMDE0MDExNVow\n" \
"PzEkMCIGA1UEChMbRGlnaXRhbCBTaWduYXR1cmUgVHJ1c3QgQ28uMRcwFQYDVQQD\n" \
"Ew5EU1QgUm9vdCBDQSBYMzCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \
"AN+v6ZdQCINXtMxiZfaQguzH0yxrMMpb7NnDfcdAwRgUi+DoM3ZJKuM/IUmTrE4O\n" \
"rz5Iy2Xu/NMhD2XSKtkyj4zl93ewEnu1lcCJo6m67XMuegwGMoOifooUMM0RoOEq\n" \
"OLl5CjH9UL2AZd+3UWODyOKIYepLYYHsUmu5ouJLGiifSKOeDNoJjj4XLh7dIN9b\n" \
"xiqKqy69cK3FCxolkHRyxXtqqzTWMIn/5WgTe1QLyNau7Fqckh49ZLOMxt+/yUFw\n" \
"7BZy1SbsOFU5Q9D8/RhcQPGX69Wam40dutolucbY38EVAjqr2m7xPi71XAicPNaD\n" \
"aeQQmxkqtilX4+U9m5/wAl0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNV\n" \
"HQ8BAf8EBAMCAQYwHQYDVR0OBBYEFMSnsaR7LHH62+FLkHX/xBVghYkQMA0GCSqG\n" \
"SIb3DQEBBQUAA4IBAQCjGiybFwBcqR7uKGY3Or+Dxz9LwwmglSBd49lZRNI+DT69\n" \
"ikugdB/OEIKcdBodfpga3csTS7MgROSR6cz8faXbauX+5v3gTt23ADq1cEmv8uXr\n" \
"AvHRAosZy5Q6XkjEGB5YGV8eAlrwDPGxrancWYaLbumR9YbK+rlmM6pZW87ipxZz\n" \
"R8srzJmwN0jP41ZL9c8PDHIyh8bwRLtTcm1D9SZImlJnt1ir/md2cXjbDaJWFBM5\n" \
"JDGFoqgCWjBH4d1QB7wCCZAA62RjYJsWvIjJEubSfZGL+T0yjWW06XyxV3bqxbYo\n" \
"Ob8VZRzI9neWagqNdwvYkQsEjgfbKbYK7p2CNTUQ\n" \
"-----END CERTIFICATE-----";

const char* ssid       = "CarletonGuests";//wlan information

const char* ntpServer = "ntp.ntsc.ac.cn";//local ntp server
const uint32_t  gmtOffset_sec = 8*3600;  //GMT+08:00
const uint16_t   daylightOffset_sec = 0;


//Since there are multiple versions of the screen, if there is a flower screen after downloading the program, please test the following four header files again!
#include <GxDEPG0213BN/GxDEPG0213BN.h>
//#include <GxGDE0213B1/GxGDE0213B1.h>      // 2.13" b/w
//#include <GxGDEH0213B72/GxGDEH0213B72.h>  // 2.13" b/w new panel
//#include <GxGDEH0213B73/GxGDEH0213B73.h>  // 2.13" b/w newer panel


#include <Fonts/FreeMonoBold18pt7b.h>


#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

#define SPI_MOSI 23
#define SPI_MISO -1
#define SPI_CLK 18

#define ELINK_SS 5
#define ELINK_BUSY 4
#define ELINK_RESET 16
#define ELINK_DC 17

#define SDCARD_SS 13
#define SDCARD_CLK 14
#define SDCARD_MOSI 15
#define SDCARD_MISO 2

#define BUTTON_PIN 39

typedef enum
{
    RIGHT_ALIGNMENT = 0,
    LEFT_ALIGNMENT,
    CENTER_ALIGNMENT,
} Text_alignment;



GxIO_Class io(SPI, /*CS=5*/ ELINK_SS, /*DC=*/ ELINK_DC, /*RST=*/ ELINK_RESET);
GxEPD_Class display(io, /*RST=*/ ELINK_RESET, /*BUSY=*/ ELINK_BUSY);

SPIClass sdSPI(VSPI);

const uint8_t Whiteboard[1700] = {0x00};

uint16_t Year = 0 , Month = 0 , Day = 0 , Hour = 0 , Minute = 0 , Second = 0;
char Date[]={"2000/01/01"};
char Time[]={"00:00:00"};
bool sdOK = false;

void displayText(const String &str, uint16_t y, uint8_t alignment)
{
  int16_t x = 0;
  int16_t x1, y1;
  uint16_t w, h;
  display.setCursor(x, y);
  display.getTextBounds(str, x, y, &x1, &y1, &w, &h);

  switch (alignment)
  {
  case RIGHT_ALIGNMENT:
    display.setCursor(display.width() - w - x1, y);
    break;
  case LEFT_ALIGNMENT:
    display.setCursor(0, y);
    break;
  case CENTER_ALIGNMENT:
    display.setCursor(display.width() / 2 - ((w + x1) / 2), y);
    break;
  default:
    break;
  }
  display.println(str);
}

void getTimeFromNTP()
{
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  Date[2] = (timeinfo.tm_year - 100) / 10 % 10 + '0';
  Date[3] = (timeinfo.tm_year - 100) % 10 + '0';
  Date[5] = (timeinfo.tm_mon + 1) / 10 % 10 + '0';
  Date[6] = (timeinfo.tm_mon + 1) % 10 + '0';
  Date[8] = timeinfo.tm_mday / 10 % 10 + '0';
  Date[9] = timeinfo.tm_mday % 10 + '0';

  Time[0] = timeinfo.tm_hour / 10 % 10 + '0';
  Time[1] = timeinfo.tm_hour % 10 + '0';
  Time[3] = timeinfo.tm_min / 10 % 10 + '0';
  Time[4] = timeinfo.tm_min % 10 + '0';
  Time[6] = timeinfo.tm_sec / 10 % 10 + '0';
  Time[7] = timeinfo.tm_sec % 10 + '0';

  Serial.println(Date);
  Serial.println(Time);
  Serial.println(" ");
}

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");

  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");

  SPI.begin(SPI_CLK, SPI_MISO, SPI_MOSI, ELINK_SS);
  display.init(); // enable diagnostic output on Serial

  display.setRotation(1);
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(&FreeMonoBold18pt7b);
  display.setCursor(0, 0);

  sdSPI.begin(SDCARD_CLK, SDCARD_MISO, SDCARD_MOSI, SDCARD_SS);

  if (!SD.begin(SDCARD_SS, sdSPI)) {
    sdOK = false;
  } else {
    sdOK = true;
  }

  display.fillScreen(GxEPD_WHITE);
  display.drawBitmap(mkrLogo, (display.width() - LOGO_WIDTH) / 2, 10,  LOGO_WIDTH, LOGO_HEIGHT, GxEPD_BLACK);
  displayText(String("Current:"), 30 + LOGO_HEIGHT, CENTER_ALIGNMENT);
  display.update();
}

int count = 1;

void loop()
{
//  getTimeFromNTP();
//  displayText(String(Date), 60, CENTER_ALIGNMENT);
//  displayText(String(Time), 90, CENTER_ALIGNMENT);
//  display.updateWindow(22, 30,  222,  90, true);
//  display.drawBitmap(Whiteboard, 22, 31,  208, 60, GxEPD_BLACK);
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
 
    if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
 
    HTTPClient http;
 
    http.begin("https://financialmodelingprep.com/api/v3/quote-short/GME?apikey=a2d72d45dbc055fbbff539492206bc5a", root_ca); //Specify the URL and certificate
    int httpCode = http.GET();                                                  //Make the request
 
    if (httpCode > 0) { //Check for the returning code

        String res = http.getString();

        Serial.println(res);
 
        StaticJsonDocument<300> doc;                         //Memory pool
        DeserializationError err = deserializeJson(doc,res); //Parse message
       
        if (err) {   //Check for errors in parsing
       
          Serial.println("Parsing failed");
          return;
       
        }
//        displayText(String("Current:"), 60, CENTER_ALIGNMENT);
//        displayText(doc[0]["price"].as<String>(), 90, CENTER_ALIGNMENT);
//        display.updateWindow(22, 30,  222,  90, true);
//        display.drawBitmap(Whiteboard, 22, 31,  208, 60, GxEPD_BLACK);

        display.fillScreen(GxEPD_WHITE);
        display.drawBitmap(mkrLogo, (display.width() - LOGO_WIDTH) / 2, 10,  LOGO_WIDTH, LOGO_HEIGHT, GxEPD_BLACK);
        displayText(String("Current:"), 30 + LOGO_HEIGHT, CENTER_ALIGNMENT);
        displayText(doc[0]["price"].as<String>(), 60 + LOGO_HEIGHT, CENTER_ALIGNMENT);
        display.update();
      }
 
    else {
      Serial.println("Error on HTTP request");
    }
 
    http.end(); //Free the resources
    
  }

//  displayText(String(count++), 60 + LOGO_HEIGHT, CENTER_ALIGNMENT);
//  display.updateWindow(20, 60 + LOGO_HEIGHT,  250,  45, true);
//  display.drawBitmap(Whiteboard, 20, 60 + LOGO_HEIGHT,  250, 45, GxEPD_BLACK);
//  Serial.println(count);
  delay(300000);
 
  }
}
