#define SPI_MOSI 23
#define SPI_MISO -1
#define SPI_CLK 18

#define ELINK_SS 5
#define ELINK_BUSY 4
#define ELINK_RESET 16
#define ELINK_DC 17

#define BUTTON_PIN 39

#include <GxEPD.h>
#include <WiFi.h>
#include <esp_wpa2.h> //wpa2 library for connections to Enterprise networks
#include <esp_wifi.h>
#include "ArduinoJson.h"
#include "time.h"
#include "mkrLogo.h"
#include <asyncHTTPrequest.h>
#include <Ticker.h>
#include "SPI.h"

#include <GxDEPG0213BN/GxDEPG0213BN.h>
#include <Fonts/FreeMonoBold12pt7b.h>


#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>


#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 22

GxIO_Class io(SPI, /*CS=5*/ ELINK_SS, /*DC=*/ ELINK_DC, /*RST=*/ ELINK_RESET);
GxEPD_Class display(io, /*RST=*/ ELINK_RESET, /*BUSY=*/ ELINK_BUSY);

const uint8_t Whiteboard[1700] = {0x00};

const char* ssid = "CarletonGuests";//wlan information

asyncHTTPrequest request;
Ticker ticker;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(12, PIN, NEO_GRB + NEO_KHZ800);

const int stops = 5;

unsigned int colors[stops][3] = {
  {200,200,255},
  {34,34,255},
  {120,34,255},
  {175,34,34},
  {255,125,0}
};

float temperature = 0;

float swellIntensity = .75;

float swellPeriod = 20;

unsigned long lastTime;
float osc = 0;

typedef enum
{
    RIGHT_ALIGNMENT = 0,
    LEFT_ALIGNMENT,
    CENTER_ALIGNMENT,
} Text_alignment;


void fade(int pxl, float val, float brt){
  if(val >= 1) val = .999;
  float space = 1. / (stops - 1);
  int which = floor(val / space);
  float amt = (val - space * which)/space;
  if(amt>=1) amt = .999;

    //ArduinoDmx0.TxBuffer[startAddr + i] = int(colors[which][i]*(1-amt)*brt + colors[which + 1][i]*amt*brt);
  int r = int(colors[which][0]*(1-amt)*brt + colors[which + 1][0]*amt*brt);
  int g = int(colors[which][1]*(1-amt)*brt + colors[which + 1][1]*amt*brt);
  int b = int(colors[which][2]*(1-amt)*brt + colors[which + 1][2]*amt*brt);
  strip.setPixelColor(pxl, strip.Color(r, g, b));
}

void setLights(){
  osc += ((millis()-lastTime)/1000.)* 2 * PI / swellPeriod;
    
  lastTime = millis();
  while(osc>2*PI) osc -= 2*PI;
  float normTemp = (temperature)/100.;
  
  //write the scaled, phased, contrained, and inverted value to the lights.
    for(int i=0; i<12; i++){
      fade(i, constrain(normTemp +cos(osc + i*PI/6) * swellIntensity/6, 0, 1),constrain((1+(1-swellIntensity)+cos(-osc*PI + i*PI/6) * swellIntensity)/2,0,1));
    }  
  strip.show();
}

void sendRequest(){
    if(request.readyState() == 0 || request.readyState() == 4){
        Serial.println("Sending request");
        request.open("GET", "http://api.openweathermap.org/data/2.5/weather?zip=55057,us&units=imperial&appid=13814bdc6b51a9b56707ceb3585f3f90");
        request.send();
    }
}

void requestCB(void* optParm, asyncHTTPrequest* request, int readyState){
    if(readyState == 4){
        String res = request->responseText();
        Serial.println(res);
        StaticJsonDocument<1028> doc;                         //Memory pool
        DeserializationError err = deserializeJson(doc, res); //Parse message
       
        if (err) {   //Check for errors in parsing
       
          Serial.println("Parsing failed");
          return;
       
        }

        display.fillScreen(GxEPD_WHITE);
        display.drawBitmap(mkrLogo, (display.width() - LOGO_WIDTH) / 2, 10,  LOGO_WIDTH, LOGO_HEIGHT, GxEPD_BLACK);
        displayText(String("Current Temp:"), 30 + LOGO_HEIGHT, CENTER_ALIGNMENT);
        displayText(doc["main"]["temp"].as<String>(), 60 + LOGO_HEIGHT, CENTER_ALIGNMENT);
        display.update();
        Serial.println(doc["main"]["temp"].as<String>());
        temperature = doc["main"]["temp"].as<float>();
        Serial.println();
    }
}

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

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");

  strip.begin();
  strip.setBrightness(50);
  strip.show();

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
  display.setFont(&FreeMonoBold12pt7b);
  display.setCursor(0, 0);

  display.fillScreen(GxEPD_WHITE);
  display.drawBitmap(mkrLogo, (display.width() - LOGO_WIDTH) / 2, 10,  LOGO_WIDTH, LOGO_HEIGHT, GxEPD_BLACK);
  displayText(String("Current:"), 30 + LOGO_HEIGHT, CENTER_ALIGNMENT);
  display.update();

  //request.setDebug(true);
  request.onReadyStateChange(requestCB);

  ticker.attach(30, sendRequest);
  ticker.attach(1/20, setLights);

  sendRequest();
}

void loop()
{
  
}
