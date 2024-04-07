#include <ESP32Servo.h>
#include <esp_now.h>
#include <WiFi.h>

// article on using servos: https://dronebotworkshop.com/esp32-servo/
// I'm using the ESP32Servo Library by Kevin Harrington https://github.com/madhephaestus/ESP32Servo
// add this using manage libraries feature (see article)

enum BodyPart {
  HIP, KNEE
};

volatile unsigned long last_message = 0;

// left to right pins (left has screw terminal outline)
// D26, D27, D14, D13, D23, D22, D21, D19
// knee0, hip0, knee1, hip1, hip2, knee2, hip3, knee3
// left to right
Servo hips[4];
Servo knees[4];
int hip_pins[4] = {27, 13, 23, 21};
int knee_pins[4] = {26, 14, 22, 19};
bool reverse_hips[8] = {0, 1, 0, 1};
bool reverse_knees[8] = {1, 1, 0, 0};

void setPos(BodyPart part, int num, int angle){
  if(part == KNEE){
    
    if(reverse_knees[num]){
      angle = 180 - angle; 
    } else {
      angle = angle;
    }

    knees[num].write(angle);
  }
  if(part == HIP){

    if(num == 0 || num == 3){
      angle = angle * 2/3 + 60;   // 60 to 180
    } else {
      angle = angle * 2/3;        // 0 to 120
    }

    if(reverse_hips[num]){
      angle = 180 - angle;
    } else {
      angle = angle;
    }
    hips[num].write(angle);
  }
}

void setKnees(int angle){
  for(int i = 0; i < 4; i++){
    setPos(KNEE, i, angle);
  }
}

void setHips(int angle){
  for(int i = 0; i < 4; i++){
    setPos(HIP, i, angle);
  }
}

// Structure example to receive data
// Must match the sender structure
struct struct_message {
  bool right_glove;
  int state;
  int hips[2];
  int knees[2];
} leftHand, rightHand;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  if(*incomingData == true){
    //right hand
    memcpy(&rightHand, incomingData, sizeof(rightHand));
  } else {
    //left hand
    memcpy(&leftHand, incomingData, sizeof(leftHand));
  }
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
 
void setup_servos() {
  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  for(int i = 0; i < 4; i++){    
    knees[i].setPeriodHertz(50);
    knees[i].attach(knee_pins[i], 500, 2500);
    hips[i].setPeriodHertz(50);
    hips[i].attach(hip_pins[i], 500, 2500);
  }
}

void setup(){
  setup_servos();
  setup_receiver();
  Serial.begin(115200);
  // while(last_message == 0){}
}

void walking_routine();
void servo_steps();
void servo_sweep();

void loop() { 
  delay(30);
  setPos(KNEE, 0, rightHand.knees[1]);
  setPos(KNEE, 1, rightHand.knees[0]);
  setPos(KNEE, 2, rightHand.knees[0]);
  setPos(KNEE, 3, rightHand.knees[1]);

  setPos(HIP, 0, rightHand.hips[1]);
  setPos(HIP, 1, rightHand.hips[0]);
  setPos(HIP, 2, rightHand.hips[0]);
  setPos(HIP, 3, rightHand.hips[1]);
}



void servo_steps(){

  // rotate by 10 degree steps routine
  for(int i = 0; i <= 180; i += 10){
     setHips(i);
     delay(1500);
  }
  delay(3000);
  for(int i = 180; i >= 0; i -= 10){
     setHips(i);
     delay(1500);
  }
  delay(3000);
  // 120 degrees  
}

void servo_sweep(){
    
  for (int pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
      for(int i = 0; i < 4; i++){
        setPos(KNEE, i, pos);
        setPos(HIP, i, pos);
      }  
    delay(15);             // waits 15ms for the servo to reach the position
  }
  for (int pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
      for(int i = 0; i < 4; i++){
        setPos(KNEE, i, pos);
        setPos(HIP, i, pos);
      }   
    delay(15);             // waits 15ms for the servo to reach the position
  }
}

void walking_routine(void){

  setKnees(170);
  setHips(150);
  delay(2000);
  setPos(KNEE, 1, 100);
  setPos(KNEE, 2, 100);
  setPos(HIP, 1, 85);
  setPos(HIP, 2, 85);
  delay(150);
  setPos(KNEE, 0, 80);
  setPos(KNEE, 3, 80);
  delay(100);
  setHips(0);
  delay(300);
  setKnees(170);

  delay(5000);

  setPos(KNEE, 0, 97);
  setPos(KNEE, 3, 97);
  setPos(HIP, 0, 100);
  setPos(HIP, 3, 100);
  delay(150);
  setPos(KNEE, 1, 60);
  setPos(KNEE, 2, 60);
  delay(200);
  setHips(150);
  delay(300);
  setKnees(170);
  delay(2000);

}

//    Serial.println("Enter number, 0-4 for knee, 5-8 for hip");  
//    while (!Serial.available()){}  
//    int num = Serial.readStringUntil('\n').toInt(); //Reading the Input string from Serial port.  
//    // Serial.printf("%d", num);
//
//    Serial.println("Enter angle");  
//    while (!Serial.available()){}  
//    int angle = Serial.readStringUntil('\n').toInt(); //Reading the Input string from Serial port.  
//    // Serial.printf("%d", angle);
//    BodyPart part;
//    if(num < 4){ 
//      part = KNEE; 
//      Serial.printf("Knee %d to %d", num, angle);
//    } else { 
//      part = HIP; 
//      num -= 4; 
//      Serial.printf("Hip %d to %d \n", num, angle);
//    }
