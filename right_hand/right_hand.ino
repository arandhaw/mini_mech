
//Code running on right hand glove
//Identical to left hand except the messages sent to the chassis are labelled differently, otherwise close to same
//Code collects data from flex sensors, puts it through a moving average filter, and sends the angle to chassis
//When the controller is turned on/reset (enable button), the code collects sensor readings for 5 seconds, and records the min and max.
//These are used to calibrate the angle calculations

#include <esp_now.h>
#include <WiFi.h>
#include <Arduino.h>

// MAC address (for bluetooth)
// we only need the MAC address of the receiving controller (chassis)
// use the get_mac_address program to get the mac address of a ESP32
// we were using two chips during testing, but only one of the below is actually used
# define MAC_B2  {0xA0, 0xA3, 0xB3, 0xAA, 0x3A, 0x74}
# define MAC_B1  {0xFC, 0xB4, 0x67, 0x73, 0xBB, 0xC0}

// set to true to print serial messages (via USB)
// this is necessary because serial print will cause the
// program to freeze if no serial is connected
bool debug = false;

// a moving average filter class
// holds @size number of data points, computes average
// used to smooth out the data
class buffer {
    private:
      int *arr;
      int size;
    public:
    // constructors
    // creates the buffer of @size, initialized to zero
    buffer(){} 
    buffer(int size){
        this->size = size;
        arr = new int[size];
        for(int i = 0; i < size; i++){
            arr[i] = 0;
        }
    }
    //destructor
    ~buffer(){
      delete arr[];
    }
    // add data to buffer
    // the buffer contains the last N variables added to it
    // adds @new_data, bumping the least recently added data point
    void add(int new_data){ 
        for(int i = size - 2; i >= 0; i--){
            arr[i + 1] = arr[i];
        }
        arr[0] = new_data;
    }
    // access ith element
    int at(int i){
        return arr[i];
    }
    // computes the average of the data
    int avg(){
        int sum = 0;
        for(int i = 0; i < size; i++){
            sum += arr[i];
        }
        return sum/size;
    }
};

buffer buff[4];
// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t recvAddress[] = MAC_B2;
esp_now_peer_info_t peerInfo;

// message struct sent to chassis
// the boolean identifies right/left hand - currently it must be first element
// any modifications must be done on both ends
struct struct_message {
  bool rightHand;
  int state;
  int hips[2];
  int knees[2];
} myData;

// callback when data is sent - empty for now
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // Serial.print("\r\nLast Packet Send Status:\t");
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// called on startup, sets up bluetooth
void init_bluetooth(void){
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

// pins for flex sensors
// order: hip0, hip1, knee0, knee1
int pins[4] = {34, 33, 35, 32};

// sensor readings corresponding to minimum and maximum angle
// set during startup calibration
int minV[4];
int maxV[4];

// called on startup
void setup() {  

  pinMode(pins[0], INPUT); 
  pinMode(pins[1], INPUT); 
  pinMode(pins[2], INPUT); 
  pinMode(pins[3], INPUT); 
  
  Serial.begin(115200);
  if(debug) Serial.println("starting calibration...");

  // calibration procedure. Basically, sets minV, maxV for each sensor
  // does this by continuously collecting data for 5 seconds and saving the max/min readings
  // these will be mapped to 0 and 180 degrees
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

// helper function for debugging
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

// gets angle from a sensor, uses a linear function to map the reading
// to a 0-180 degree range.
// the sensor readings are between 0-4096, measure voltage at pins
// @num - 0 to 3, identifies the pin
int getAngle(int num){
  int x;
  if(num == 0){
    x = (buff[0].avg() - minV[0])*180/(maxV[0] - minV[0]);
  }
  else if(num == 1){
    x = 180 - (buff[1].avg() - minV[1])*180/(maxV[1] - minV[1]);
  }
  else if(num == 2){
    x = (buff[2].avg() - minV[2])*180/(maxV[2] - minV[2]);
  }
  else if(num == 3){
    x = (buff[3].avg() - minV[3])*180/(maxV[3] - minV[3]);
  }
  if(x < 0){ x = 0;}
  if(x > 180){x = 180;};
  return x;
}

// main controller loop
void loop() {
  // collect sensor readings every 1ms for 30ms
  for(int i = 0; i < 30; i++){
    buff[0].add(analogRead(pins[0]));
    buff[1].add(analogRead(pins[1]));
    buff[2].add(analogRead(pins[2]));
    buff[3].add(analogRead(pins[3]));
    delay(1);
  }
  // after collecting 30 data points, compose message for chassis
  // the average of the buffer is used in getAngle function
  myData.rightHand = true;
  myData.state = 0;   // we'll have to agree on what different states mean
  myData.hips[0] = getAngle(0);     // for each hip
  myData.hips[1] = getAngle(1);     // for each hip
  myData.knees[0] = getAngle(2);     // for each knee
  myData.knees[1] = getAngle(3);     // for each knee
  
  // Send message via ESP-NOW
  esp_err_t result1 = esp_now_send(recvAddress, (uint8_t *) &myData, sizeof(myData));
  
  if(debug) printOutput();
}
