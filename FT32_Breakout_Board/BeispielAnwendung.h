// BeispielAnwendung.h
#include "ft_ESP32_IOobjects.h"
#include "task.h"

#ifndef _BEISPIELSANWENDUNG_h
#define _BEISPIELSANWENDUNG_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

typedef enum : char {	//enum mit atomarem Zugriff f�r Threadsafety
	BEISPIEL_STATE_PAUSE,
	BEISPIEL_STATE_STOP,
	BEISPIEL_STATE_RUN
} e_BeispielState_t;

typedef struct {
	e_BeispielState_t state;
	unsigned char step;
	Motor mMotors[4];
} st_BeispielSHM_e;



class BeispielAnwendung {
public:
	BeispielAnwendung();
	~BeispielAnwendung();
	void start();
	void pause();
	void stop();
	unsigned int getStep();
private:
	//bool mPause;
	//bool mStop;
	st_BeispielSHM_e *mSHM;		//SHM mit Run-Task
	void *mTask;				//task Handle
};


#endif

