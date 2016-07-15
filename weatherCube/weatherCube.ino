#include <ESP8266WiFi.h>

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

void setup() {
  Serial.begin(115200);
  delay(100);
  wifiConnect();
}
 
void loop() {
  Serial.println(httpGet());
  yield();
}
