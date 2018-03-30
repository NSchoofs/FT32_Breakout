/*
Ausgangstreiber für ESP32-Fischertechnik-Anbindung
Autor: Johannes Marquart
*/
#ifndef FT_ESP32_IOOBJECTS_H
#define FT_ESP32_IOOBJECTS_H

#include <Arduino.h>
#include "SparkFunSX1509.h"

//Festlegen Anzahl Ports
constexpr size_t MOTOR_QTY = 4;	//Anzahl der Motoren
constexpr size_t LAMP_QTY = 4;	//Anzahl der Lampen
constexpr size_t DAIN_QTY = 7;	//Anzahl der Eingänge (digital/analog)
constexpr size_t DIO_PWMO_QTY = 8;

//Zuweisung Ports (von Ursprungsprojekt)
/*const int PORT_M_DIR[MOTOR_QTY] = { 17, 4, 14, 12 };	//Output-Pins Motor-Richtung
const int PORT_M_PWM[MOTOR_QTY] = { 16, 2, 13, 15 };	//Output-Pins Motor-Drehzahl
const int PIN_M_INH = 27;	//Output-Pin Einschalten Motortreiber

const int PORT_L_PWM[LAMP_QTY] = { 16, 2, 13, 15 };	//Output-Pins Lampe, werden hier über den selben Treiber angesteuert
const int PIN_L_INH = 27;	//Output-Pin Einschalten Lampentreiber

const int PORT_IN[DAIN_QTY] = { 39, 34, 35, 32, 33, 25, 26 };	//Input-Pins Ditital/Analog*/

//Zuweisung Ports auf SX1509
const byte SX1509PORT_M_DIR[MOTOR_QTY] = { 0, 2, 4, 6 };	//Motor Richtung - Output
const byte SX1509PORT_M_PWM[MOTOR_QTY] = { 1, 3, 5, 7 };	//Output-Pins Motor-Drehzahl
const byte PIN_M_INH = 27;	//Output-Pin Einschalten Motortreiber

const byte PORT_L_PWM[LAMP_QTY] = { 04, 00, 02, 15 };	//Output-Pins Lampe, werden hier über den selben Treiber angesteuert
const byte PIN_L_INH = 27;	//Output-Pin Einschalten Lampentreiber

const byte PORT_IN[DAIN_QTY] = { 39, 34, 35, 32, 33, 25, 26 };	//Input-Pins Ditital/Analog*/

const byte SX1509PORT_DIO_PWMO[DIO_PWMO_QTY] = { 8, 9, 10, 11, 12, 13, 14, 15 };	//DIO/PWMout Pins auf SX1509

//SX1509 Variables
const byte SX1509_I2C_ADDRESS = 0x3E;		//wenn keine ADD-Jumper auf SX1509 aktiviert sind
const byte SX1509_I2C_PIN_SDA = 21;
const byte SX1509_I2C_PIN_SCL = 22;
const byte SX1509_PIN_RESET = 16;
const byte SX1509_PIN_EXTI = 17;
static bool initSX1509 = false;

class Motor
{
public:
	Motor();	//Standardkonstruktor, setzt alles auf 0;
	Motor(unsigned int motorNr);	//Konstruktor, Motor-Nr (0..3), weist zu: Pin-Nr für PWM, Pin-Nr für Richtung
	void setValues(bool, unsigned int);	//neue Motorwerte setzen (Richtung, Drehzahl)
	void reRun();	//bei Aufruf werden erneut die Pins und PWM mit den Attributen gesetzt
	//evtl. eine Methode Stop einbauen
private:
	unsigned int mMotorNr;	//Motornummer 0..3, wird bei Erstellung des Objekts angelegt
	unsigned int mPortNrPWM;	//Portnummer für PWM, wird bei Erstellung des Objekts zugewiesen
	unsigned int mPortNrDir;	//PortNr für Richtung, wird bei Erstellung des Objekts zugewiesen
	bool mRechtslauf;	//Drehrichtung: rechts = true, links = false
	unsigned int mDrehzahl;	//aktuelle Geschwindigkeit (von 0 bis 8)
};

class Lampe
{
public:
	Lampe();
	Lampe(unsigned int);	//Konstruktor, Lampe-Nr (0..7), weist zu: Pin-Nr für PWM
	void setValues(unsigned int);	//neue Lampenwerte setzen (Aktiv, Helligkeit)
	void reRun();	//bei Aufruf werden erneut die Pins und PWM mit den Attributen gesetzt
private:
	unsigned int mLampeNr;	//LampenNr 0..7, wird bei Erstellung des Objekts angelegt
	unsigned int mPortNrPWM;	//Portnummer für PWM, wird bei Erstellung des Objekts zugewiesen
	unsigned int mHelligkeit;	//aktuelle Helligkeit (von 0 bis 8)
};

class Encoder
{
public:

private:

};

class DigitalAnalogIn
{
public:
	DigitalAnalogIn();
	DigitalAnalogIn(unsigned int);
	unsigned int getValueAnalog();
	unsigned int getValueDigital();
	void setValueDigital(bool);	//Digitalen Ausgang setzen (HIGH/LOW)
private:
	unsigned int mInputNummer;
	unsigned int mInputPortNr;
};

//Klasse um zusätzliche IOs vom SX1509 zu nutzen
class DigitalIO_PWMout 
{
public:
	DigitalIO_PWMout(byte io, byte inOut);		//io von 0-7, inOut-Constants von ARDUINO nutzen
	bool getValue();
	void setValueDig(bool val);
	void setPWM(unsigned char pwmVal);
private:
	DigitalIO_PWMout();
	byte mIOPin;
	byte mIONumber;
	byte mDirection;
};

#endif