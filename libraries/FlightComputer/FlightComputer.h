#ifndef FLIGHT_COMPUTER_H
#define FLIGHT_COMPUTER_H

#include <WProgram.h>
#include <QuadCopter.h>
#include <AutoControl.h>
#include <SignalListener.h>
#include <RemoteControl.h>
#include <UltraSoundSignal.h>
#include <AccelerometerSignal.h>

class FlightComputer {
    
public:
    
    // values for the PID controller
    static const AutoControl::Configuration HOVER_CONF;
    static const AutoControl::Configuration LANDING_CONF;
    static const AutoControl::Configuration ACCEL_CONF;    
    
    // Flight computer states
    typedef enum {GROUND=0, HOVER, LANDING, FAILED, EMERGENCY_LANDING, MANUAL_CONTROL, ENGAGING_AUTO_CONTROL} State;
        
    // delay between readings of the ultra sound module
    static const int MIN_TIME_ULTRA_SOUND = 100;
    
    // delay between readings of the accelerometers
    static const int MIN_TIME_ACCEL = 50;
    
    // delay between status messages
    static const int MIN_TIME_STATUS_MESSAGE = 5000;
    
    // min/max for the automatic control of the throttle
    static const double MIN_THROTTLE = QuadCopter::MIN_SPEED/4;
    static const double MAX_THROTTLE = QuadCopter::MAX_SPEED/2;
    
    // min/max for the automatic control of the aileron and elevator
    static const double MIN_TILT = QuadCopter::MIN_SPEED/2;
    static const double MAX_TILT = QuadCopter::MAX_SPEED/2;    
    
    // landings will cut the power once this height is reached
    static const double THROTTLE_OFF_HEIGHT = 10;
    
    // throttle setting for when we don't know the height anymore
    static const int EMERGENCY_DESCENT = -20;
    
    FlightComputer(QuadCopter&, RemoteControl&, UltraSoundSignal&, AccelerometerSignal&, AccelerometerSignal&);
    ~FlightComputer() {}
    
    void init();
    void takeoff(long);
    void hover(long);
    void land();
    void emergencyDescent();
    void manualControl();
    void autoControl();
    void abort();
    void adjust();
    void stabilize(bool);
    void log();
    void setHoverConfiguration(const AutoControl::Configuration &conf);
    void setLandingConfiguration(const AutoControl::Configuration &conf);
    void setStabilizerConfiguration(const AutoControl::Configuration &conf);
    
private:
    // values for the PID controller
    AutoControl::Configuration hoverConf;
    AutoControl::Configuration landingConf;
    AutoControl::Configuration accelConf;
    
    // Listener to update the height of the flight computer
    class HeightListener : public SignalListener {
    public:
        HeightListener(FlightComputer &comp) : comp(comp) {};
        virtual void update(double x, long time) {
            comp.height = x;
        }
        FlightComputer &comp;
    };
    
    // Listener to update the longitudinal force on the flight computer
    class LongitudinalListener : public SignalListener {
    public:
        LongitudinalListener(FlightComputer &comp) : comp(comp) {};
        virtual void update(double x, long time) {
            comp.longitudinalForce = x;
        }
        FlightComputer &comp;
    };

    // Listener to update the longitudinal force on the flight computer
    class LateralListener : public SignalListener {
    public:
        LateralListener(FlightComputer &comp) : comp(comp) {};
        virtual void update(double x, long time) {
            comp.lateralForce = x;
        }
        FlightComputer &comp;
    };    
    
    // adjusts output from PID controller for throttle setting
    class ThrottleControl : public ControlListener {
    public:
        ThrottleControl(QuadCopter &ufo) : ufo(ufo), currentThrottle(QuadCopter::MIN_SPEED) {};
        virtual void adjust(double x) {
            currentThrottle += x;
            if (currentThrottle > MAX_THROTTLE) {
                currentThrottle = MAX_THROTTLE;
            } else if (currentThrottle < MIN_THROTTLE) {
                currentThrottle = MIN_THROTTLE;
            }
            ufo.throttle((int)currentThrottle);
        }
        double currentThrottle;
        QuadCopter &ufo;
    };
    
    // adjusts output from PID controller for elevator setting
    class ElevatorControl : public ControlListener {
    public:
        ElevatorControl(QuadCopter &ufo) : ufo(ufo), currentElevator(QuadCopter::STOP_SPEED) {};
        virtual void adjust(double x) {
            currentElevator = x;
            if (currentElevator > MAX_TILT) {
                currentElevator = MAX_TILT;
            } else if (currentElevator < MIN_TILT) {
                currentElevator = MIN_TILT;
            }            
            ufo.elevator((int)currentElevator);
        }
        double currentElevator;
        QuadCopter &ufo;
    };

    // adjusts output from PID controller for aileron setting
    class AileronControl : public ControlListener {
    public:
        AileronControl(QuadCopter &ufo) : ufo(ufo), currentAileron(QuadCopter::STOP_SPEED) {};
        virtual void adjust(double x) {
            currentAileron = x;
            if (currentAileron > MAX_TILT) {
                currentAileron = MAX_TILT;
            } else if (currentAileron < MIN_TILT) {
                currentAileron = MIN_TILT;
            }                        
            ufo.aileron((int)currentAileron);
        }
        double currentAileron;
        QuadCopter &ufo;
    };
    
    QuadCopter &ufo; // quad copter
    RemoteControl &rc; // RC signal (from RC controller)
    
    UltraSoundSignal &ultraSound; // distance pointing down
    AccelerometerSignal &longitudinalAccel; // accel on y axis
    AccelerometerSignal &lateralAccel; // accel on x axis
    
    ThrottleControl throttleControl; // sets the throttle
    ElevatorControl elevatorControl; // sets the elevator
    AileronControl aileronControl; // sets the aileron
    
    HeightListener heightListener; // updates height of computer
    LateralListener lateralListener; // updates forward force of the computer
    LongitudinalListener longitudinalListener; // updates sideways force of the computer
    
    AutoControl autoThrottle; // autopilot for throttle
    AutoControl autoElevator; // autopilot for elevator
    AutoControl autoAileron; // autopilot for aileron
    
    State state;
    
    double height;
    double zeroHeight;
    
    double longitudinalForce;
    double zeroLongitudinalForce;
    
    double lateralForce;
    double zeroLateralForce;
    
    long time;
    long lastTimeHeightSignal;
    long lastTimeAccelSignal;
    long lastTimeLog;
};

#endif
