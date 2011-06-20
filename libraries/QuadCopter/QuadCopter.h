#ifndef QUADCOPTER_H
#define QUADCOPTER_H

#include "WProgram.h"
#include "Servo.h"

class QuadCopter {
public:
	
	typedef enum {LONGITUDINAL=0, LATERAL=1, VERTICAL=2, ROTATIONAL=3} Direction;
	/*
		Colour map for GU-344 gyro pins (available with GAUI 330X)
		aileronPin;    // White 
		rudderPin;     // Yellow 
		throttlePin;   // Orange 
		elevatorPin;   // Red 
		gainPin;       // Green (Gain/Gear) 
	*/
	QuadCopter(int aileronPin, int rudderPin, int throttlePin, int elevatorPin, int gainPin);
	~QuadCopter() {}
	void init();
	void move(int x, int y, int z, int r);
	void move(int[]);
	void move(Direction,int);
	void stop(Direction);
	void stop();
	int read(Direction d) {return speed[d];};
	int (&read())[4] {return speed;};
	void throttle(int speed) {move(VERTICAL,speed);}
	void elevator(int speed) {move(LONGITUDINAL,speed);}
	void aileron(int speed) {move(LATERAL,speed);}
	void rudder(int speed) {move(ROTATIONAL,speed);}
	void adjustGain(int);
	
	static const int DEGREES_OF_FREEDOM = 4;
	static const int MIN_SPEED = -100;
	static const int STOP_SPEED = 0;
	static const int MAX_SPEED = 100;

private:
	
	void arm();
	void attach();
	
	Servo servos[DEGREES_OF_FREEDOM];
	Servo gain;
	
	int speed[DEGREES_OF_FREEDOM];
	
	int pins[DEGREES_OF_FREEDOM];
	int gainPin;
};

#endif
