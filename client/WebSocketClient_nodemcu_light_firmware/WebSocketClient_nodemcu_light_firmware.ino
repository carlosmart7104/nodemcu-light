#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

StaticJsonBuffer<200> jsonBuffer;

unsigned long tiempo = millis(), anterior = millis();

#define USE_SERIAL Serial

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {


    switch(type) {
        case WStype_TEXT:
            USE_SERIAL.printf("%s\n", payload);
            break;
        case WStype_BIN:
            USE_SERIAL.printf("%u\n", length);
            hexdump(payload, length);
            break;
    }

}
  
void setup() {
    pinMode(A0, INPUT);
    
    USE_SERIAL.begin(115200);
    
      for(uint8_t t = 4; t > 0; t--) {
          USE_SERIAL.flush();
          delay(1000);
      }

    char json[] = "{\"ssid\":\"SSID\",\"pass\":\"PASSWORD\",\"host\":\"HOST_NAME_OR_IP\",\"port\":8000}";
    while(1){
      JsonObject& root = jsonBuffer.parseObject(json);
      delay(500);
      if(root.success()){
        Serial.println("boot");
        const char *ssid = root["ssid"], *pass = root["pass"];
        WiFiMulti.addAP(ssid, pass);
        
        WiFi.disconnect();
        while(WiFiMulti.run() != WL_CONNECTED) {
           USE_SERIAL.printf("."); 
           delay(100);
        }
        
        USE_SERIAL.printf("\n");
        const char* host = root["host"];
        int port = root["port"];
        webSocket.begin(host, port);
        webSocket.onEvent(webSocketEvent);
        break;
      } else {
        Serial.println("JSON invalido");
        ESP.restart();
      }
    }
}

void loop() {
  tiempo = millis();
  int r = analogRead(A0);
  char buff[4];
  String s = String(r);
  Serial.print("send value: " + s + " - ");
  s.toCharArray(buff,4);
  webSocket.sendTXT(buff);
  webSocket.loop();
  tiempo = millis() - tiempo;
  Serial.println("time: " + String(tiempo) + "ms");
}
