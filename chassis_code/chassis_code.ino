#include <ESP32Servo.h>
#include <esp_now.h>
#include <WiFi.h>

// article on using servos: https://dronebotworkshop.com/esp32-servo/
// I'm using the ESP32Servo Library by Kevin Harrington https://github.com/madhephaestus/ESP32Servo
// add this using manage libraries feature (see article)

enum BodyPart {
  HIP, KNEE
};
// Knee/Hip servos are ordered left to right
Servo hips[4];
Servo knees[4];
int hip_pins[4] = {27, 13, 23, 21};
int knee_pins[4] = {26, 14, 22, 19};
// We attempted to keep the following convention: 
// Knees: 0 degrees is fully retracted, 180 extended
// Hips: Different from each hip. 0 degrees is legs pointed backwards, 180 degrees pulled forward.
bool reverse_hips[8] = {0, 1, 1, 0};
bool reverse_knees[8] = {1, 1, 0, 0};

// function to set the position of the motors 
// @part KNEE or HIP
// @num the motor number, from 0 to 3. Numbered from left to right.
// @angle An angle between 0 and 180 degrees.
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

    if(reverse_hips[num]){
      angle = 180 - angle;
    }
    
    angle = angle * 165/180;
    hips[num].write(angle);
  }
}

// Unused in working code, just for testing
// Set angle of all knees at once
void setKnees(int angle){
  for(int i = 0; i < 4; i++){
    setPos(KNEE, i, angle);
  }
}
// Unused in working code, just for testing
// Set angle of all hips
void setHips(int angle){
  for(int i = 0; i < 4; i++){
    setPos(HIP, i, angle);
  }
}

// Struct received via bluetooth from gloves
// Must match the sender structure
struct struct_message {
  bool right_glove;
  int state;
  int hips[2];
  int knees[2];
} leftHand, rightHand;

// callback function that will be executed when data is received
// I believe this is triggered by interrupts
// copies the data into right_hand/left_hand based on whether the 
// first boolean of the message is true/false
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  // This is a bit of a hack
  // *incomingData deferences the first byte of data
  // This happens to be the boolean "right_glove" that identifies the glove 
  // as left/right
  if(*incomingData == true){
    //right hand
    memcpy(&rightHand, incomingData, sizeof(rightHand));
  } else {
    //left hand
    memcpy(&leftHand, incomingData, sizeof(leftHand));
  }
}

// Sets up the bluetooth library
// Called on startup
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

// Sets up the servos
void setup_servos() {
  // Allow allocation of all timers
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  
  // sets the frequency and PWM ranges, and attaches the pins of each servo
  for(int i = 0; i < 4; i++){    
    knees[i].setPeriodHertz(50);
    knees[i].attach(knee_pins[i], 500, 2500);
    hips[i].setPeriodHertz(50);
    hips[i].attach(hip_pins[i], 500, 2500);
  }
}

// Called on startup
void setup(){
  setup_servos();
  setup_receiver();
  Serial.begin(115200);   // for debugging
  // while(last_message == 0){}
}

void walking_routine();
void servo_steps();
void servo_sweep();

// Main loop of controller
void loop() { 

  delay(15);
  // Disable interrupts, and copy data to struct rH, lH
  // the reason for this convoluted chain of copies is to avoid race conditions
  portDISABLE_INTERRUPTS();   
  struct_message rH = rightHand;
  struct_message lH = leftHand;
  portENABLE_INTERRUPTS();
  
  setPos(KNEE, 3, lH.knees[0]);
  setPos(KNEE, 2, lH.knees[1]);
  setPos(KNEE, 1, rH.knees[0]);
  setPos(KNEE, 0, rH.knees[1]);

  setPos(HIP, 3, lH.hips[0]);
  setPos(HIP, 2, lH.hips[1]);
  setPos(HIP, 1, rH.hips[0]);
  setPos(HIP, 0, rH.hips[1]);
}


// the rest of these functions were just for debugging
// they move the legs in various routines

// this one sweeps the servos over 180 degrees in 10 degree steps
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

// move all the servos in a 180 degree sweep
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

// Make the servo walk itself
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
