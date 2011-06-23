#include "Servo.h"
#include "SerialController.h"
#include "QuadCopter.h"
#include "Tokenizer.h"
#include "HardwareReader.h"

int aileronPin = 12; //White 
int rudderPin = 9; //Yellow 
int throttlePin = 10;  //Orange 
int elevatorPin = 11; //Red 
int gainPin = 7;  //Green (Gain/Gear)
int killPin = 8; // LOW kills the flight

HardwareReader r(Serial);
SerialController controller(aileronPin, rudderPin, throttlePin, 
                            elevatorPin, gainPin, killPin, r);

void setup() {
    Serial.begin(9600);
    controller.init();
}

void loop() {
    controller.executeCommand();
}
