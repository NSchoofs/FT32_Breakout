// 
// 
// 

#include "BeispielAnwendung.h"

void RunBeispielAnwendung(void* args) {
	st_BeispielSHM_e *mSHM = (st_BeispielSHM_e*)args;
	bool initPause = false;

	while (1) {
		/*if (//hier check ob Cody queue gefüllt ist)	
		{
			mSHM->state = BEISPIEL_STATE_PAUSE;
		}*/

		else if (mSHM->state == BEISPIEL_STATE_PAUSE) {		//alle Motoren Stoppen
			if (!initPause) {
				for (char i = 0; i < 4; i++)
					mSHM->mMotors[i].setValues(1, 0);
				initPause = true;
			}
		}

		else if (mSHM->state == BEISPIEL_STATE_STOP) {		//zurück zum Start, dann startbereit machen
			mSHM->step = 0;
			mSHM->state = BEISPIEL_STATE_PAUSE;
		}

		else if (mSHM->state == BEISPEL_STATE_RUN) {
			initPause = false;
			switch (mSHM->step)
			{
			case 0:
				break;
			case 1:
				break;
			case 2: 
				break;
				
				//...

			default:
				break;
			}
		}
		delay(10);			//mind. 10 milisek warten bis nächster durchlauf
	}
}

BeispielAnwendung::BeispielAnwendung()
{
	for (char i = 0; i < 4; i++)
	{
		mSHM->mMotors[i] = Motor(i);
	}

	xTaskCreatePinnedToCore(
		RunBeispielAnwendung,   				/* Function to implement the task */
		"BeispielAnwendung", 	/* Name of the task */
		1024,      				/* Stack size in words */
		(void*)mSHM,      		/* Task input parameter */
		0,          			/* Priority of the task */
		mTask,       			/* Task handle. */
		1);  					/* Core where the task should run */
}

BeispielAnwendung::~BeispielAnwendung()
{
	vTaskDelete(mTask);
}

void BeispielAnwendung::start()
{
	mSHM->state = BEISPIEL_STATE_RUN;
}

void BeispielAnwendung::pause()
{
	mSHM->state = BEISPIEL_STATE_PAUSE;
}

void BeispielAnwendung::stop()
{
	mSHM->state = BEISPIEL_STATE_STOP;
}

unsigned int BeispielAnwendung::getStep()
{
	return mSHM->step;
}
