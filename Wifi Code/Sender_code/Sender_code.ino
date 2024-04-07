#include <esp_now.h>
#include <WiFi.h>

# define MAC_B2  {0xA0, 0xA3, 0xB3, 0xAA, 0x3A, 0x74}
# define MAC_B1  {0xFC, 0xB4, 0x67, 0x73, 0xBB, 0xC0}

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t recvAddress[] = MAC_B1;
esp_now_peer_info_t peerInfo;

// data to send - change inside as needed
struct struct_message {
  int state;
  int hips[4];
  int knees[4];
} myData;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // Serial.print("\r\nLast Packet Send Status:\t");
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void init_bluetooth(void ){
  WiFi.mode(WIFI_STA); // Set device as a Wi-Fi Station
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    // Serial.println("Error initializing ESP-NOW");
    return;
  }
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  // Register peer
  memcpy(peerInfo.peer_addr, recvAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    // Serial.println("Failed to add peer");
    return;
  }
}

void setup() {  
  // Serial.begin(115200);
  init_bluetooth();
}
 
void loop() {
  // Set values to send
  myData.state = 0;   // we'll have to agree on what different states mean
  myData.hips[0] = 150;     // for each hip
  myData.knees[0] = 67;     // for each knee
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(recvAddress, (uint8_t *) &myData, sizeof(myData));
  if (result == ESP_OK) {
    // Serial.println("Sent with success");
  }
  else {
    // Serial.println("Error sending the data");
  }
  delay(2000);
}
