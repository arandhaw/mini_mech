//Code running on left hand glove
//Identical to right hand except for small differences
//This file is not documented since it is so similar to right_hand
// see right_hand file to read a fully documented version

#include <esp_now.h>
#include <WiFi.h>
#include <Arduino.h>

# define MAC_B2  {0xA0, 0xA3, 0xB3, 0xAA, 0x3A, 0x74}
# define MAC_B1  {0xFC, 0xB4, 0x67, 0x73, 0xBB, 0xC0}

bool debug = false;

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t recvAddress[] = MAC_B2;
esp_now_peer_info_t peerInfo;

class buffer {
    public:
    int size;
    int *arr;
    buffer(){}
    buffer(int size){
        this->size = size;
        arr = new int[size];
        for(int i = 0; i < size; i++){
            arr[i] = 0;
        }
    }
    void add(int addition){
        for(int i = size - 2; i >= 0; i--){
            arr[i + 1] = arr[i];
        }
        arr[0] = addition;
    }
    int at(int i){
        return arr[i];
    }
    int avg(){
        int sum = 0;
        for(int i = 0; i < size; i++){
            sum += arr[i];
        }
        return sum/size;
    }
};

buffer buff[4];

struct struct_message {
  bool rightHand;
  int state;
  int hips[2];
  int knees[2];
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
//


// hip0, hip1, knee0, knee1
int pins[4] = {34, 33, 35, 32};


int minV[4];
int maxV[4];


void setup() {  

  pinMode(pins[0], INPUT); 
  pinMode(pins[1], INPUT); 
  pinMode(pins[2], INPUT); 
  pinMode(pins[3], INPUT); 
  
  Serial.begin(115200);
  if(debug) Serial.println("starting calibration...");
  
  for(int i = 0; i < 4; i++){
    minV[i] = 4096;
    maxV[i] = 0;
  }
  
  unsigned long startTime = millis();
  while(millis() - startTime < 5000){
    for(int i = 0; i < 4; i++){
      int x = analogRead(pins[i]);
      if(x > maxV[i]){
        maxV[i] = x;
      }
      if(x < minV[i]){
        minV[i] = x;
      }
    }
  }

  init_bluetooth();
  for(int i = 0; i < 4; i++){
    buff[i] = buffer(30);
  }
}

void printOutput(){
  Serial.println("Hip angle, knee angle");
  Serial.println(myData.hips[0]);
  Serial.println(myData.hips[1]);
  Serial.println(myData.knees[0]);
  Serial.println(myData.knees[1]);
  Serial.println();
//  Serial.println("Analog in");
//  Serial.println(analogRead(pins[0]));
//  Serial.println(analogRead(pins[1]));
//  Serial.println(analogRead(pins[2]));
//  Serial.println(analogRead(pins[3]));

  Serial.println();
}

// bend is down
int getAngle(int num){
  int x;
  if(num == 0){
    x = (buff[0].avg() - minV[0])*180/(maxV[0] - minV[0]);
  }
  if(num == 1){
    x = 180 - (buff[1].avg() - minV[1])*180/(maxV[1] - minV[1]);
  }
  if(num == 2){
    x = (buff[2].avg() - minV[2])*180/(maxV[2] - minV[2]);
  }
  if(num == 3){
    x = (buff[3].avg() - minV[3])*180/(maxV[3] - minV[3]);
  }
  if(x < 0){ x = 0;}
  if(x > 180){x = 180;};
  return x;
}


void loop() {
  for(int i = 0; i < 30; i++){
    buff[0].add(analogRead(pins[0]));
    buff[1].add(analogRead(pins[1]));
    buff[2].add(analogRead(pins[2]));
    buff[3].add(analogRead(pins[3]));
    delay(1);
  }
  myData.rightHand = false;
  myData.state = 0;
  myData.hips[0] = getAngle(0);     // for each hip
  myData.hips[1] = getAngle(1);     // for each hip
  myData.knees[0] = getAngle(2);     // for each hip
  myData.knees[1] = getAngle(3);     // for each hip
  // Send message via ESP-NOW
  esp_err_t result2 = esp_now_send(recvAddress, (uint8_t *) &myData, sizeof(myData));
  if(debug) printOutput();
}
