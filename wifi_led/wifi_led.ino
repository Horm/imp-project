/**
 * Source file for NodeMCU LED project...
 * @author Matej Horak (xhorak68) - 100%
 * @date 30.12.2018
**/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "FS.h"
#include <Arduino.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Constants
const int ledPinRed = 5; // GPIO5---D1 of NodeMCU
const int ledPinOrange = 4; // GPIO4---D2 of NodeMCU
const int ledPinGreen = 0; // GPIO0---D3 of NodeMCU
const String ssid = "LED_WIFI_AP";
const String password = "ledWifiApPass";

// State variables...
String ledMode = "normal";
int effectNumber = 0;
bool trafficPrevious, trafficCurrent = false;
bool red, orange, green = false;

// Web Server
AsyncWebServer server(80); // Port 80

void setup() {
  Serial.begin(115200);
  delay(10);

  if(!SPIFFS.begin()){
     Serial.println("An Error has occurred while mounting SPIFFS");
     return;
  }

  initLeds();

  initWifiAP();

  initWebServer();

}

void loop() {

  if (ledMode == "traffic") {
    if (wasTrafficStateChanged()) {
      handleTraffic();
    }
  }
  else if (ledMode == "effects") {
    runEffectSequence(effectNumber);
  }
  else {
    // Normal mode - do nothing
  }

}

void initLeds() {
  pinMode(ledPinRed, OUTPUT);
  digitalWrite(ledPinRed, LOW);

  pinMode(ledPinOrange, OUTPUT);
  digitalWrite(ledPinOrange, LOW);

  pinMode(ledPinGreen, OUTPUT);
  digitalWrite(ledPinGreen, LOW);
}

void initWifiAP() {
  WiFi.mode(WIFI_AP);
  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(ssid.c_str(), password.c_str()) ? "Ready" : "Failed!");

  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
}

void initWebServer() {
  
  initLedAPIRequests();
  
  initWebFileRequests();
  
  server.begin();
  Serial.println("Web server started!");
}

void initLedAPIRequests() {
  
  server.on("/normal/red", HTTP_GET, [](AsyncWebServerRequest *request){
    ledMode = "normal";
    switchRed();
    request->send(200, "application/json", getNormalJson());
  });
  
  server.on("/normal/orange", HTTP_GET, [](AsyncWebServerRequest *request){
    ledMode = "normal";
    switchOrange();
    request->send(200, "application/json", getNormalJson());
  });

  server.on("/normal/green", HTTP_GET, [](AsyncWebServerRequest *request){
    ledMode = "normal";
    switchGreen();
    request->send(200, "application/json", getNormalJson());
  });

  server.on("/normal", HTTP_GET, [](AsyncWebServerRequest *request){
    ledMode = "normal";
    initNormal();
    request->send(200, "application/json", getNormalJson());
  });

  server.on("/traffic/off", HTTP_GET, [](AsyncWebServerRequest *request){
    ledMode = "traffic";
    trafficPrevious = trafficCurrent;
    trafficCurrent = false;
    request->send(200, "application/json", "{\"status\": false}");
  });

  server.on("/traffic/on", HTTP_GET, [](AsyncWebServerRequest *request){
    ledMode = "traffic";
    trafficPrevious = trafficCurrent;
    trafficCurrent = true;
    request->send(200, "application/json", "{\"status\": true}");
  });

  server.on("/traffic", HTTP_GET, [](AsyncWebServerRequest *request){
    ledMode = "traffic";
    initTrafficLights();
    trafficPrevious, trafficCurrent = false;
    request->send(200, "application/json", "{\"status\": false}");
  });

  server.on("/effects/1", HTTP_GET, [](AsyncWebServerRequest *request){
    ledMode = "effects";
    initEffects();
    effectNumber = 1;
    request->send(200, "application/json", "{\"status\": false}");
  });

  
  server.on("/effects/2", HTTP_GET, [](AsyncWebServerRequest *request){
    ledMode = "effects";
    initEffects();
    effectNumber = 2;
    request->send(200, "application/json", "{\"status\": false}");
  });
  

  server.on("/effects/3", HTTP_GET, [](AsyncWebServerRequest *request){
    ledMode = "effects";
    initEffects();
    effectNumber = 3;
    request->send(200, "application/json", "{\"status\": false}");
  });


  server.on("/effects", HTTP_GET, [](AsyncWebServerRequest *request){
    initEffects();
    ledMode = "effects";
    effectNumber = 0;
    request->send(200, "application/json", "{\"status\": false}");
  });
}

void initWebFileRequests() {
  
  server.on("/css/icon.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/icon.css", "text/css");
  });
  
  server.on("/css/materialize.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/materialize.css", "text/css");
  });

  server.on("/css/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/style.css", "text/css");
  });

  server.on("/js/init.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/js/init.js", "text/javascript");
  });

  server.on("/js/materialize.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/js/materialize.js", "text/javascript");
  });
  
  server.on("/js/axios.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/js/axios.min.js", "text/javascript");
  });

  server.on("/js/jquery-2.1.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/js/jquery-2.1.1.min.js", "text/javascript");
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });
}

String getBooleanString(bool value) {
  return value ?  "true" : "false";
}

// --------------------------- NORMAL ---------------------------

void initNormal() {
  digitalWrite(ledPinOrange, LOW);
  digitalWrite(ledPinGreen, LOW);
  digitalWrite(ledPinRed, LOW);

  red = false;
  orange = false;
  green = false;
}

void switchRed() {
  if (red) {
    digitalWrite(ledPinRed, LOW);
  }
  else {
    digitalWrite(ledPinRed, HIGH);
  }
    
  red = !red;
}

void switchOrange() {
  if (orange) {
    digitalWrite(ledPinOrange, LOW);
  }
  else {
    digitalWrite(ledPinOrange, HIGH);
  }
  orange = !orange;
}

void switchGreen() {
  if (green) {
    digitalWrite(ledPinGreen, LOW);
  }
  else {
    digitalWrite(ledPinGreen, HIGH);
  }
  green = !green;
}

String getNormalJson() {
  return "{\"redStatus\": "+ getBooleanString(red) + ", \"orangeStatus\": " + getBooleanString(orange) + ", \"greenStatus\": " + getBooleanString(green) + "}";
}

// --------------------------- EFFECTS ---------------------------

void initTrafficLights() {
  digitalWrite(ledPinOrange, LOW);
  digitalWrite(ledPinGreen, LOW);
  digitalWrite(ledPinRed, HIGH);
}

void trafficLightsOn() {
  digitalWrite(ledPinOrange, HIGH);
  delay(2000);
  digitalWrite(ledPinOrange, LOW);
  digitalWrite(ledPinRed, LOW);
  digitalWrite(ledPinGreen, HIGH);
}


void trafficLightsOff() {
  digitalWrite(ledPinGreen, LOW);
  digitalWrite(ledPinOrange, HIGH);
  delay(2000);
  digitalWrite(ledPinRed, HIGH);
  digitalWrite(ledPinOrange, LOW);

}

bool wasTrafficStateChanged() {
  return trafficPrevious != trafficCurrent;
}

void handleTraffic() {
  if (trafficCurrent == true) {
    trafficLightsOn();
  }
  else {
    trafficLightsOff();
  }
  trafficPrevious = trafficCurrent;
}


// --------------------------- EFFECTS ---------------------------

void initEffects() {
  digitalWrite(ledPinOrange, LOW);
  digitalWrite(ledPinGreen, LOW);
  digitalWrite(ledPinRed, LOW);
}

void runEffectSequence(int effectNumber) {
  switch (effectNumber) {
      case 1: {
        effect1();
        break;
      }
      case 2: {
        effect2();
        break;
      }
      case 3: {
        effect3();
        break;
      }
    }
}


void effect1() {
  digitalWrite(ledPinOrange, HIGH);
  digitalWrite(ledPinGreen, HIGH);
  digitalWrite(ledPinRed, HIGH);
  delay(500);
  digitalWrite(ledPinOrange, LOW);
  digitalWrite(ledPinGreen, LOW);
  digitalWrite(ledPinRed, LOW);
  delay(500);
}

void effect2() {
  digitalWrite(ledPinRed, HIGH);
  delay(100);
  digitalWrite(ledPinRed, LOW);
  digitalWrite(ledPinOrange, HIGH);
  delay(100);
  digitalWrite(ledPinOrange, LOW);
  digitalWrite(ledPinGreen, HIGH);
  delay(100);
  digitalWrite(ledPinGreen, LOW);
}

int mapLedToPin(int number) {
  if (number == 1) {
    return ledPinRed;
  }
  else if (number == 2) {
    return ledPinOrange;
  }
  else if (number == 3) {
    return ledPinGreen;
  }
  else {
    return -1;
  }
  
}

void effect3() {

  int randOn = mapLedToPin(random(1,4));
  int randOff = mapLedToPin(random(1,4));

  digitalWrite(randOn, HIGH);
  digitalWrite(randOff, LOW);
  
  delay(150);
}
