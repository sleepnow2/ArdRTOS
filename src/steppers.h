#ifndef __STEPPER_H__
#define __STEPPER_H__

#include <Arduino.h>

const double pi2 = 2*PI;
/*
 ######  ######## ######## ########  ########  ######## ########
##    ##    ##    ##       ##     ## ##     ## ##       ##     ##
##          ##    ##       ##     ## ##     ## ##       ##     ##
 ######     ##    ######   ########  ########  ######   ########
      ##    ##    ##       ##        ##        ##       ##   ##
##    ##    ##    ##       ##        ##        ##       ##    ##
 ######     ##    ######## ##        ##        ######## ##     ##
*/
class Stepper {
protected:
	uint8_t dPin, sPin;
	int64_t targStep = 0;
	int64_t currStep = 0;
public:
	Stepper () {
		dPin = 0;
		sPin = 0;
	}

	Stepper (uint8_t sPin, uint8_t dPin) {
		this->dPin = dPin;
		this->sPin = sPin;
	}

	void begin() {
		pinMode(dPin, OUTPUT);
		pinMode(sPin, OUTPUT);
	}

	void update() {
		if (targStep != currStep) {
			int8_t dir = ((targStep > currStep) ? 1 : -1);
			currStep = currStep + dir;
			digitalWrite(dPin, (dir == 1));
			delayMicroseconds(1);

			digitalWrite(sPin, 1);
			delayMicroseconds(1);
			digitalWrite(sPin, 0);
		}
	}

    int64_t distFromTargStep() {
        return targStep - currStep;
    }

	void Zero() {
		currStep = 0;
		targStep = 0;
	}

	void absTargStep(int64_t trg) {
		targStep = trg;
	}
	void relTargStep(int64_t trg) {
		targStep += trg;
	}
};
/*
   ###    ########  ##     ## ######## ######## ##     ## ########  ########
  ## ##   ##     ## ###   ### ##          ##    ##     ## ##     ## ##
 ##   ##  ##     ## #### #### ##          ##    ##     ## ##     ## ##
##     ## ########  ## ### ## ######      ##    ##     ## ########  ######
######### ##   ##   ##     ## ##          ##    ##     ## ##   ##   ##
##     ## ##    ##  ##     ## ##          ##    ##     ## ##    ##  ##
##     ## ##     ## ##     ## ########    ##     #######  ##     ## ########
*/
class Armeture: public Stepper {
protected:
	int64_t stepsPerRot;

public:

    Armeture() {
        this->dPin = 0;
		this->sPin = 0;
		this -> stepsPerRot = 200;
    }

	Armeture(uint8_t sPin, uint8_t dPin, int64_t stepsPerRot) {
		this->dPin = dPin;
		this->sPin = sPin;
		this -> stepsPerRot = stepsPerRot;
	}

	void absTargRad(double rad) {
		absTargStep((int64_t)(rad/pi2*stepsPerRot));
	}
	void relTargRad(double rad) {
		relTargStep((int64_t)(rad/pi2*stepsPerRot));
	}

    double distFromTargRad() {
        return ((double)distFromTargStep())/(double)stepsPerRot*pi2;
    }
};

#endif // !__STEPPER_H__