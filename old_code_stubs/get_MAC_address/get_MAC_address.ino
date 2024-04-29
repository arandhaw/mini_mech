// Complete Instructions to Get and Change ESP MAC Address: https://RandomNerdTutorials.com/get-change-esp32-esp8266-mac-address-arduino/

// get MAC address of the plugged in microcontroller
// MAC is a unique address for every network device on the planet
// needed for bluetooth
// We only need the receiver microcontrollers MAC address (chassis)
#include "WiFi.h"
 
void setup(){
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  Serial.println(WiFi.macAddress());
}
 
void loop(){

}
