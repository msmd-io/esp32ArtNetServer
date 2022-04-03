/*
This example will transmit a universe via Art-Net into the Network.
This example may be copied under the terms of the MIT license, see the LICENSE file for details
*/
#include <ArtnetWifi.h>
#include <Arduino.h>
#include <map>

//Wifi settings
const char* ssid = "Art-Net"; // CHANGE FOR YOUR SETUP
const char* password = "esp32artnetAP"; // CHANGE FOR YOUR SETUP

// Artnet settings
ArtnetWifi artnet0;
ArtnetWifi artnet1;

const int startUniverse = 0; // CHANGE FOR YOUR SETUP most software this is 1, some software send out artnet first universe as 0.
const char host0[] = "2.0.0.2"; // CHANGE FOR YOUR SETUP your destination
const char host1[] = "2.0.0.3"; // CHANGE FOR YOUR SETUP your destination

struct RGB {
    byte r;
    byte g;
    byte b;
};

RGB defaultColours[] = {
  {255,0,0},
  {0,255,0},
  {0,0,255},
  {255, 255, 0},
  {0,255, 255}
};

RGB currentColour = {0,0,0};
bool bChangeColour = true;
int artnetRefreshIntervalMS = 40;
int artnetLastRefreshTime = millis();
int microphoneInterruptPin = 4;

void IRAM_ATTR isr() {
  bChangeColour = true;
}

// connect to wifi – returns true if successful or false if not
boolean ConnectWifi(void)
{
  boolean state = true;
  int i = 0;

  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");
  
  // Wait for connection
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20){
      state = false;
      break;
    }
    i++;
  }
  if (state){
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("Connection failed.");
  }
  
  return state;
}

void changeColour() {
  currentColour = defaultColours[random(sizeof(defaultColours))];

  for (int i=0; i<61; i++) {
    artnet0.setByte(i*3+0, currentColour.r);
    artnet0.setByte(i*3+1, currentColour.g);
    artnet0.setByte(i*3+2, currentColour.b);
  }
  for (int i=0; i<30; i++) {
    artnet1.setByte(i*3+0, currentColour.r);
    artnet1.setByte(i*3+1, currentColour.g);
    artnet1.setByte(i*3+2, currentColour.b);
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(microphoneInterruptPin, INPUT);
  attachInterrupt(microphoneInterruptPin, isr, RISING);
  ConnectWifi();
  artnet0.begin(host0);
  artnet0.setLength(3*61);
  artnet0.setUniverse(startUniverse);
  artnet1.begin(host1);
  artnet1.setLength(3*30);
  artnet1.setUniverse(startUniverse);
}

void loop() {
  if (bChangeColour) {
    Serial.println("Changing Colour");
    changeColour();
    bChangeColour = false;
  }
  if (millis() - artnetLastRefreshTime >= artnetRefreshIntervalMS) {
    artnet1.write();
    artnet0.write();
    artnetLastRefreshTime = millis();
  }

  delay(1);
}
