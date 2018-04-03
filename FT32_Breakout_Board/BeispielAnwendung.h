// BeispielAnwendung.h
#include "ft_ESP32_IOobjects.h"
#include "ft_ESP32_SHM.h"


#ifndef _BEISPIELSANWENDUNG_h
#define _BEISPIELSANWENDUNG_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

typedef enum : char {	//enum mit atomarem Zugriff für Threadsafety
	BEISPIEL_STATE_PAUSE,
	BEISPIEL_STATE_STOP,
	BEISPIEL_STATE_RUN
} e_BeispielState_t;

typedef struct {
	e_BeispielState_t state;
	unsigned char step;
	Motor mMotors[4];
	SHM *ptrSHMQueue;
} st_BeispielSHM_e;



class BeispielAnwendung {
public:
	//BeispielAnwendung();
	BeispielAnwendung(SHM *ptrSHMQueueArg);		//benötigt SHM ptr zum SW-Queue SHM um zu überprüfen, ob Cody++ gerade aktiv ist --> Beispielanwendung wird gestoppt
	~BeispielAnwendung();
	void start();
	void pause();
	void stop();
	unsigned int getStep();
private:
	//bool mPause;
	//bool mStop;
	st_BeispielSHM_e *mSHM;		//SHM mit Run-Task
	TaskHandle_t *mTask;				//task Handle
};


#endif

