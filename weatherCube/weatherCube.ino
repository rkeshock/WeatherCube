#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <SevSeg.h>
 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, 1, NEO_GRB + NEO_KHZ800);
SevSeg sevseg;

static unsigned long timer = millis();

void wifiConnect(){ //connect to wifi
  const char* ssid     = "ssid";
  const char* password = "password";
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
  yield();
  return;
}

String httpGet(){ //retrieve json file as string
  const char* host = "api.openweathermap.org";
  
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    return "";
  }
  String url = "/data/2.5/weather?id=4508722&units=imperial&APPID=apikey";
  
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(500);

  int n = 0;
  while(client.available()){
    String line = client.readStringUntil('\r');
    n++;
    if(n==15){
      yield();
      return line;
    }
  }
  
  yield();
  return "";
}

float parseJson(String jsonString){
  char json[jsonString.length() + 1];
  jsonString.toCharArray(json, jsonString.length() + 1);
  yield();
  float temp = 0;
  StaticJsonBuffer<800> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);
  yield();  
  if (!root.success()) {
    return 9999; //return error
  }
  temp = root["main"]["temp"];
  yield();
  return temp;
}

void setSevSeg(){
  byte numDigits = 4;
  byte digitPins[] = {9, 5, 4, 2};
  byte segmentPins[] = {10, 0, 12, 15, 3, 16, 14, 13};
  sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins);
  sevseg.setNumber(0, 1);
  sevseg.refreshDisplay();
  yield();
  return;
}

static void glow(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i  , c); // Draw new pixel
  }
  return;
}

void setColor(float temp){
    if(temp > 90.0){
      glow(strip.Color(255, 0, 0)); // Red
    }
    if(temp > 80 && temp <= 90){
      glow(strip.Color(255, 82, 0));
    }
    if(temp > 70 &&  temp <= 80){
      glow(strip.Color(255, 165, 0));
    }
    if(temp > 60 &&  temp <= 70){
      glow(strip.Color(255, 210, 0));
    }
    if(temp > 50 &&  temp <= 60){
      glow(strip.Color(255, 255, 0));
    }
    if(temp > 40 &&  temp <= 50){
      glow(strip.Color(255, 255, 127));
    }
    if(temp > 30 &&  temp <= 40){
      glow(strip.Color(200, 200, 255));
    }
    if(temp > 20 &&  temp <= 30){
      glow(strip.Color(163, 195, 255));
    }
    if(temp > 10 &&  temp <= 20){
      glow(strip.Color(127, 191, 255));
    }
    if(temp <= 10){
      glow(strip.Color(179, 127, 255));
    }
    yield();
    return;
}

void setup() {
  //general setup
  wifiConnect();
  strip.begin();
  setSevSeg();
  delay(500);

  //initial color setting
  float temp = parseJson(httpGet());
  delay(500);
  setColor(temp);
  strip.setBrightness(64);
  sevseg.setNumber(temp, 1);
  strip.show();
  yield();
}
 
void loop() {
  if (millis() >= timer + 600000) {
    float temp = parseJson(httpGet());
    if(temp != 9999){ //error handling
      setColor(temp);
      sevseg.setNumber(temp, 1);
      strip.show();
    }
    timer = millis();
  }
  sevseg.refreshDisplay();
}
