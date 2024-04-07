#include <esp_now.h>
#include <WiFi.h>

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  int state;
  int hips[4];
  int knees[4];
} struct_message;

// Create a struct_message called myData
struct_message myData;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("State");
  Serial.println(myData.state);
  Serial.println("Hip angles:");
  Serial.println(myData.hips[0]);
  Serial.println(myData.hips[1]);
  Serial.println(myData.hips[2]);
  Serial.println(myData.hips[3]);
  Serial.println(myData.state);
  Serial.println("Hip angles:");
  Serial.println(myData.knees[0]);
  Serial.println(myData.knees[1]);
  Serial.println(myData.knees[2]);
  Serial.println(myData.knees[3]);
}

void setup_receiver(){
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}

void setup() {
  setup_receiver();
}
 
void loop() {

}
