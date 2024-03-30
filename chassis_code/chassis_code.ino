#include <ESP32Servo.h>

// article on using servos: https://dronebotworkshop.com/esp32-servo/
// I'm using the ESP32Servo Library by Kevin Harrington https://github.com/madhephaestus/ESP32Servo
// add this using manage libraries feature (see article)

enum BodyPart {
  HIP, KNEE
};

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
  int pos;
  if(part == KNEE){
    
    if(reverse_knees[num]){
      pos = 180 - angle; 
    } else {
      pos = angle;
    }

    knees[num].write(pos);
  }
  if(part == HIP){
    if(reverse_hips[num]){
      pos = 180 - angle;
    } else {
      pos = angle;
    }

    hips[num].write(pos);
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
 
void setup() {
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
  Serial.begin(115200);
}


void loop() { 
    Serial.println("Enter number, 0-4 for knee, 5-8 for hip");  
    while (!Serial.available()){}  
    int num = Serial.readStringUntil('\n').toInt(); //Reading the Input string from Serial port.  
    // Serial.printf("%d", num);

    Serial.println("Enter angle");  
    while (!Serial.available()){}  
    int angle = Serial.readStringUntil('\n').toInt(); //Reading the Input string from Serial port.  
    // Serial.printf("%d", angle);
    BodyPart part;
    if(num < 4){ 
      part = KNEE; 
      Serial.printf("Knee %d to %d", num, angle);
    } else { 
      part = HIP; 
      num -= 4; 
      Serial.printf("Hip %d to %d \n", num, angle);
    }

    setPos(part, num, angle);
    
    
//  for (int pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
//    // in steps of 1 degree
//      for(int i = 0; i < 8; i++){
//        setPos(i , pos);
//      }  
//    delay(15);             // waits 15ms for the servo to reach the position
//  }
//  for (int pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
//      for(int i = 0; i < 8; i++){
//        setPos(i, pos);
//      }   
//    delay(15);             // waits 15ms for the servo to reach the position
//  }
  
//  setKnees(10);
//  delay(2000);
//  setKnees(170);
//  delay(2000);
//  setHips(10);
//  delay(2000);
//  setHips(170);

//  delay(2000);
//  setKnees(180);
//  setHips(100);
//  delay(2000);
//  setPos(HIP, 1, 120);
//  setPos(HIP, 4, 120);
//  setPos(HIP, 2, 80);
//  setPos(HIP, 3, 80);
//  delay(2000);
//  setPos(HIP, 2, 120);
//  setPos(HIP, 3, 120);
//  setPos(HIP, 1, 80);
//  setPos(HIP, 4, 80);
//  delay(2000);
//  setHips(100);
//  delay(2000);
//  setKnees(0);
}
