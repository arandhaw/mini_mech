#include <Arduino.h>

// put function declarations here:

/* #define SENSOR_1 39
#define SENSOR_2 36
#define SENSOR_3 34
#define SENSOR_4 35 */

#define SENSOR_1 32
#define SENSOR_2 33
#define SENSOR_3 25
#define SENSOR_4 26

void setup() {
  // put your setup code here, to run once:
  pinMode(SENSOR_1, INPUT);
  pinMode(SENSOR_2, INPUT);
  pinMode(SENSOR_3, INPUT);
  pinMode(SENSOR_4, INPUT);
  Serial.begin(9600);
}

char output[100];

void loop() {
  // put your main code here, to run repeatedly:
  int input1 = analogRead(SENSOR_1);
  int input2 = analogRead(SENSOR_2);
  int input3 = analogRead(SENSOR_3);
  int input4 = analogRead(SENSOR_4);
  sprintf(output, "%i,%i,%i,%i", input1, input2, input3, input4);
  Serial.println(output);
}
