/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0;    // variable to store the servo position

void setup() {
  //Serial.begin(9600);
  myservo.attach(7);  // attaches the servo on pin 9 to the servo object
}

void loop() {
    //long value = analogRead(A5);
    //Serial.println(value);
    //long pos = value*180/1023;
    //Serial.print("Servo angle ");
    //Serial.println(pos);
    myservo.write(0);              // tell servo to go to position in variable 'pos'
    delay(3000);      // waits 15ms for the servo to reach the position
    myservo.write(180);
    delay(3000);
}
