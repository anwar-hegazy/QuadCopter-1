#include <Memory.h>
#include <Servo.h>
#include <SerialController.h>
#include <QuadCopter.h>
#include <Tokenizer.h>
#include <HardwareReader.h>
#include <CommandParser.h>
#include <FlightComputer.h>
#include <FlightComputerCommandParser.h>
#include <DistanceListener.h>
#include <UltraSoundSignal.h>

int aileronPin = 12; //White 
int rudderPin = 9; //Yellow 
int throttlePin = 10;  //Orange 
int elevatorPin = 11; //Red 
int gainPin = 7;  //Green (Gain/Gear)
int killPin = 8; // LOW kills the flight
int pingPin = 13; // ultrasound sensor

HardwareReader reader(Serial);
QuadCopter ufo(aileronPin, rudderPin, throttlePin, elevatorPin, gainPin);
UltraSoundSignal distance(pingPin);
FlightComputer computer(ufo);
FlightComputerCommandParser parser(computer);
SerialController controller(parser, killPin, reader);

void setup() {
    Serial.begin(9600);
    ufo.init();
    computer.init();
    controller.init();
    distance.registerListener(&computer);
}

void loop() {
    controller.executeCommand();
    distance.signal();
    computer.adjust();
}
