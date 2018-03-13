// SX1509Handler.h

#ifndef _SX1509HANDLER_h
#define _SX1509HANDLER_h




class SX1509Handler {
public:
	SX1509Handler(char i2cSDA, char i2cSCL);
	bool setPWM();
	bool setDO();
	bool setAO();	//fake AI (only PWM simulated analog value)
	bool readDI();
	float readAI();
private:

};








#endif

