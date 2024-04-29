## Repository for Mini-Mech capstone 
##### Project by Christian Turpin, Armaan Randhawa, Henry Mullock, Reese Critchlow
##### Written by Armaan Randhawa, May 2024

Contains all the code run on the three ESP32 microcontrollers - chassis, right_glove, and left_glove. The code for each is in the folders
above, and can be uploaded via USB-C.

### Brief code summary:

The flex sensor readings are recorded by the gloves and sent via bluetooth every 30ms. A moving average filter was added to remove noise.
The chassis updates the location of the servo motors accordingly. When the gloves are first turned on, a calibration procedure records
the minimum/maximum values from flex sensor which are used to map the readings linearly to an angle.
Code_stubs contains some older code that was useful during development. 
To run on a different ESP32, the get_MAC_address program must be run to get the MAC address of the chassis. 
The glove code then must be updated so that they know the chassis MAC_address.

See the code and final report for further documentation.

### Dependencies: 
Everything was coded in the arduino IDE. The ESPNow bluetooth library and a servo library were used.

Article on using servos: https://dronebotworkshop.com/esp32-servo/            \
I'm using the ESP32Servo Library by Kevin Harrington https://github.com/madhephaestus/ESP32Servo            \
This library must be added in the Arduino IDE using the manage libraries feature.
ESP32 Pinout: https://lastminuteengineers.com/esp32-pinout-reference/                    

