#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

//change these to your ssid and password
const char* ssid     = "ssid";
const char* password = "password";

const char* host = "api.openweathermap.org";
float temp = 0;

void setup() {
  Serial.begin(115200);
  delay(100);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

float tempRequest(float temp){
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return temp;
  }
  
  // We now create a URI for the request: change appid to your api key
  String url = "/data/2.5/weather?id=4508722&units=imperial&APPID=11111111";
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(500);

  StaticJsonBuffer<800> jsonBuffer;
  String jsonString;
  
  // Read all the lines of the reply from server and store line 15 as json
  int n = 0;
  while(client.available()){
    String line = client.readStringUntil('\r');
    n++;
    if(n==15){
      jsonString = line;
    }
  }
  
  Serial.println(jsonString);
  Serial.println("closing connection");

  char json[jsonString.length() + 1];
  jsonString.toCharArray(json, jsonString.length() + 1);
  JsonObject& root = jsonBuffer.parseObject(json);
  if (!root.success()) {
    Serial.println("parseObject() failed");
    return temp;} //returning from here

  temp = root["main"]["temp"];
  //Serial.println(temp);
  Serial.println();
  return temp;
}

void loop() {
  temp = tempRequest(temp);
  Serial.println("Returned value of ");
  Serial.print(temp);
  Serial.println();
  Serial.println();
  delay(20000);
}
