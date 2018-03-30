#include "ft_ESP32_queueWorker.h"
//#include <Adafruit_SSD1306.h>
//extern Adafruit_SSD1306 display;

bool queueWorker(commonElement*& startPtr, commonElement*& endPtr, array<Motor,MOTOR_QTY>& mMotorArray, array<Lampe, LAMP_QTY>& mLampeArray, array<DigitalAnalogIn, DAIN_QTY>& mDAIn, SHM* mSHM)
{	
	Serial.println("Queue abarbeiten...");	//Debug-Ausgabe... alle Seriellen Ausgaben sind zur Pr�fung des Programmablaufs in der Konsole
	
	commonElement* workPtr = startPtr->nextElement;

  // (workPtr != endPtr) && !mSHM->commonStopp
	while ((workPtr != endPtr) && mSHM->commonStart)	//wiederholen bis das letzte QE erreicht oder Stopp gedr�ckt wurde
	{

		int waitTime = 0;	//Variable zur Wartezeit [ms]

		switch (workPtr->ID)
		{
		case 'M':
			mMotorArray.at(workPtr->portNr).setValues(workPtr->compare_direction, workPtr->val_pwm_timems_loop);	//Motordaten.set, Ausgangspins werden gesetzt
			workPtr = workPtr->nextElement;	//Zeiger auf n�chstes QE legen
			break;
		case 'L':
			mLampeArray.at(workPtr->portNr).setValues(workPtr->val_pwm_timems_loop); //Lampendaten.set -> sollte sofort ausgefuehrt werden
			workPtr = workPtr->nextElement;	//Zeiger auf naechstes QE legen
			break;
		case 'D':	//Handling der Digitalen Anschluesse
			if (workPtr->type == 'O')	//Digitalen Ausgang setzen
			{
				mDAIn.at(workPtr->portNr).setValueDigital(workPtr->compare_direction);	//Ein-/Ausschalten des Pins
			}
			else	//Digitalen Eingang lesen, wird von der HMI noch nicht verwendet
			{
				mSHM->digitalVal[workPtr->portNr] = mDAIn.at(workPtr->portNr).getValueDigital();	//Wert in Container schreiben
				//Wenn das Display zur Analoganzeige benutzt wird:
				/*
				display.setCursor(0,0);
				display.print("Digital In ");
				display.println(workPtr->portNr);
				display.print(mDAIn.at(workPtr->portNr).getValueDigital());
				*/
				//Ende Display
			}
			workPtr = workPtr->nextElement;	//Zeiger auf n�chstes QE legen
			break;
		case 'A':	//wird von der HMI noch nicht verwendet
			mSHM->analogVal[workPtr->portNr] = mDAIn.at(workPtr->portNr).getValueAnalog()/32;	//Wert in Container schreiben, 12Bit-Wert auf 7Bit-Wert skalieren (Beschr�nkung wegen HMI)
			//Wenn das Display zur Digitalanzeige benutzt wird:
			/*
			display.setCursor(0,16);
			display.print("Analog In ");
			display.println(workPtr->portNr);
			display.print(mDAIn.at(workPtr->portNr).getValueAnalog());
			*/
			//Ende Display
			workPtr = workPtr->nextElement;	//Zeiger auf n�chstes QE legen
		case 'S':
			waitTime = workPtr->time_s * 1000;	//Sekunden in warten [ms] speichern
			waitTime += workPtr->val_pwm_timems_loop * 10;	//Millisekunden in warten [ms] speichern
			Serial.print("Warte ");
			Serial.print(waitTime);
			Serial.println(" ms");
			workPtr = workPtr->nextElement;	//Zeiger auf n�chstes QE legen
			break;
		case 'I':
			if (workPtr->type == 'D')	//Digitalen Eingang abfragen
			{
				Serial.print("If-Verzweigung Digital: ");
				if ((bool)mDAIn.at(workPtr->portNr).getValueDigital() == (bool)workPtr->val_pwm_timems_loop)	//Wenn Taster gedr�ckt wurde, in if reingehen
				{
					Serial.println(" Gehe in if rein");
					workPtr = workPtr->nextElement;	//in If reingehen
				}
				else	//Taster wurde nicht gedr�ckt, springen
				{
					Serial.println("Ueberspringe if");
					if(workPtr->jumpElement->ID == 'E')	//existiert ein Else-Pfad? (Pr�fung nur f�r debugging)
					{
						Serial.println("Gehe in Else");
					}
					workPtr = workPtr->jumpElement->nextElement;	//Else-Fall, Zeiger auf eins nach Else-/Endif-Element legen
				}
			}
			else	//Analogen Eingang abfragen
			{
				unsigned int measure_val = mDAIn.at(workPtr->portNr).getValueAnalog()/32;	//Einlesen des Analogwertes, 12Bit-Wert auf 7Bit-Wert skalieren (Beschr�nkung wegen HMI)
				Serial.print("If-Verzweigung Analog: ");
				switch (workPtr->compare_direction)	//Auswahl, welcher Vergleichsoperator durchgef�hrt wird
				{
				case '<':	//Ain < Vergleichswert
					if (measure_val < workPtr->val_pwm_timems_loop)	//Wenn Ain < Vergleichswert, in if reingehen
					{
						Serial.println("Gehe in if rein");
						workPtr = workPtr->nextElement;	//in If reingehen
					}
					else	//Ain nicht < Vergleichswert
					{
						Serial.println("Ueberspringe if");
						if(workPtr->jumpElement->ID == 'E')	//existiert ein Else-Pfad? (Pr�fung nur f�r debugging)
						{
							Serial.println("Gehe in Else");
						}
						workPtr = workPtr->jumpElement->nextElement;	//Else-Fall, Zeiger auf eins nach Else-/Endif-Element legen
						//workPtr = workPtr->jumpElement;	//Zeiger auf zugeh�riges Endif legen
					}
					break;
				case '>':	//Ain > Vergleichswert
					if (measure_val > workPtr->val_pwm_timems_loop)	//Wenn Ain > Vergleichswert, in if reingehen
					{
						Serial.println("Gehe in if rein");
						workPtr = workPtr->nextElement;	//in If reingehen
					}
					else	//Ain nicht > Vergleichswert
					{
						Serial.println("Ueberspringe if");
						if(workPtr->jumpElement->ID == 'E')	//existiert ein Else-Pfad? (Pr�fung nur f�r debugging)
						{
							Serial.println("Gehe in Else");
						}
						workPtr = workPtr->jumpElement->nextElement;	//Else-Fall, Zeiger auf eins nach Else-/Endif-Element legen
						//workPtr = workPtr->jumpElement;	//Zeiger auf zugeh�riges Endif legen
					}
					break;
				case '=':
					if (measure_val == workPtr->val_pwm_timems_loop)	//Wenn Ain gleich Vergleichswert, in if reingehen
					{
						Serial.println("Gehe in if rein");
						workPtr = workPtr->nextElement;	//in If reingehen
					}
					else	//Ain ungleich Vergleichswert
					{
						Serial.println("Ueberspringe if");
						if(workPtr->jumpElement->ID == 'E')	//existiert ein Else-Pfad? (Pr�fung nur f�r debugging)
						{
							Serial.println("Gehe in Else");
						}
						workPtr = workPtr->jumpElement->nextElement;	//Else-Fall, Zeiger auf eins nach Else-/Endif-Element legen
						//workPtr = workPtr->jumpElement;	//Zeiger auf zugeh�riges Endif legen
					}
					break;
				default:
					break;
				}
			}
			break;
		case 'E':	//Else
			workPtr = workPtr->jumpElement->nextElement;	//nach If-Pfad wird Else-Pfad �bersprungen. Zeiger auf eins nach zugeh�rigem Endif legen
			break;
		case 'J':	//Endif
			workPtr = workPtr->nextElement;	//hier passiert nix, Endif wird �bersprungen
			break;
		case 'W':
			if (workPtr->type == 'Z')
			{
				Serial.print("Zaehlschleife: ");
				if (workPtr->val_pwm_timems_loop > 0) {	//Anzahl der Durchl�ufe > 0
					Serial.print("noch ");
					Serial.print(workPtr->val_pwm_timems_loop);
					Serial.println(" Durchlaeufe. Betrete Schleife");
					workPtr->val_pwm_timems_loop--;	//Anzahl der noch offenen Schleifendurchl�ufe um 1 reduzieren 
					workPtr = workPtr->nextElement;	//in While Reingehen 
				}
				else
				{
					Serial.println("Ueberspringe Schleife");
					workPtr = workPtr->jumpElement->nextElement;
				}
			}
			else if (workPtr->type == 'D')	//Digitalen Eingang abfragen
			{
				Serial.print("Schleife, digital Abfragen: ");
				if ((bool)mDAIn.at(workPtr->portNr).getValueDigital() == (bool)workPtr->val_pwm_timems_loop)	//Wenn Taster gedr�ckt wurde, in if reingehen
				{
					Serial.println("Betrete Schleife");
					workPtr = workPtr->nextElement;	//in While reingehen
				}
				else	//Taster wurde nicht gedr�ckt, springen
				{
					Serial.println("Ueberspringe Schleife");
					workPtr = workPtr->jumpElement->nextElement; //Zeiger auf Element nach zugeh�rigem EndWhile legen
				}
			}
			else	//Analogen Eingang abfragen
			{
				unsigned int measure_val = mDAIn.at(workPtr->portNr).getValueAnalog()/32;	//Einlesen des Analogwertes, 12Bit-Wert auf 7Bit-Wert skalieren (Beschr�nkung wegen HMI)
				Serial.print("Schleife, analog Abfragen: ");
				switch (workPtr->compare_direction)	//Auswahl, welcher Vergleichsoperator durchgef�hrt wird
				{
				case '<':	//Ain < Vergleichswert
					if (measure_val < workPtr->val_pwm_timems_loop)	//Wenn Ain < Vergleichswert, in if reingehen
					{
						Serial.println("Betrete Schleife");
						workPtr = workPtr->nextElement;	//in If reingehen
					}
					else	//Ain nicht < Vergleichswert
					{
						Serial.println("Ueberspringe Schleife");
						workPtr = workPtr->jumpElement->nextElement;	//Zeiger auf zugeh�riges Endif legen
					}
					break;
				case '>':	//Ain > Vergleichswert
					if (measure_val > workPtr->val_pwm_timems_loop)	//Wenn Ain > Vergleichswert, in if reingehen
					{
						Serial.println("Betrete Schleife");
						workPtr = workPtr->nextElement;	//in If reingehen
					}
					else	//Ain nicht > Vergleichswert
					{
						Serial.println("Ueberspringe Schleife");
						workPtr = workPtr->jumpElement->nextElement;	//Zeiger auf zugeh�riges Endif legen
					}
					break;
				case '=':
					if (measure_val == workPtr->val_pwm_timems_loop)	//Wenn Ain gleich Vergleichswert, in if reingehen
					{
						Serial.println("Betrete Schleife");
						workPtr = workPtr->nextElement;	//in If reingehen
					}
					else	//Ain ungleich Vergleichswert
					{
						Serial.println("Ueberspringe Schleife");
						workPtr = workPtr->jumpElement->nextElement;	//Zeiger auf zugeh�riges Endif legen
					}
					break;
				default:
					break;
				}
			}
			break;
		case 'X':	//EndWhile
			Serial.println("Wiederhole Schleife");
			workPtr = workPtr->jumpElement;	//Springt zur�ck zu While und dortiger Pr�fung
			break;
		default:
			break;
		}
		
		//DEBUG MIT DISPLAY - Anzeige aller Eingaenge (digital und analog)
		/*
		for(int i = 0; i < DAIN_QTY; i++)
		{
			display.setCursor((i%4)*32,(i/4)*16);
			display.print(PORT_IN[i]);
			display.print(" ");
			display.print(!mDAIn.at(i).getValueDigital());
			display.setCursor((i%4)*32,((i/4)*16)+8);
			//display.print(mDAIn.at(i).getValueAnalog());
		}
		display.display();
		display.clearDisplay();
		*/
		//~DEBUG MIT DISPLAY

		do {
			if (waitTime > 0)	//wenn Wartezeit vorhanden
			{
				//Serial.println("Warte 10 ms");
				waitTime -= 10;	//Wartezeit um 10ms reduzieren
				delay(10);
			}
			if (mSHM->commonPause)	//Pause-Button wurde gedr�ckt
			{
				Serial.println("Pause gestartet");
				Motor mMotorSave[mMotorArray.size()];	//Sicherungsobjekte f�r Motoren anlegen
				Lampe mLampeSave[mLampeArray.size()];	//Sicherungsobjekte f�r Lampen anlegen
				for (int i = 0; i < mMotorArray.size(); i++)
				{
					mMotorSave[i] = mMotorArray[i];	//Motor i sichern
					mMotorArray[i].setValues(true, 0);	//Ausgang zur 0 setzen
					mLampeSave[i] = mLampeArray[i];	//Lampe i sichern
					mLampeArray[i].setValues(0);	//Ausgang zur 0 setzen
				}
				do {	//in Schleife auf Aufhebung des Pausezustandes warten
					delay(10);	//10ms warten (schohnt CPU)
				} while (mSHM->commonPause && mSHM->commonStart);	//warten auf Aufhebung der Pause oder auf Stopp
       
				Serial.println("Pause beendet");
				//IO-Objekte auf kopierte Werte zur�cksetzen
				for (int i = 0; i < mMotorArray.size(); i++)
				{
					mMotorArray[i] = mMotorSave[i];	//Ausgangswerte zur�cksetzen
					mMotorArray[i].reRun();	//Ausgang neu setzen
					mLampeArray[i] = mLampeSave[i];
					//mLampeArray[i].reRun();
				}
			}
		} while (waitTime > 0 && mSHM->commonStart);	//warten auf Ende der Pausenzeit oder auf Stopp
    //waitTime > 0 && !mSHM->commonStopp
		
	}

	Serial.println("Queue abarbeiten Ende.\n");
	
	Serial.println("Ausgaenge zuruecksetzen...");
	
	for (int i = 0; i < mMotorArray.size(); i++)	//Am Ende des Programms zur�cksetzen der Ausg�nge auf 0
	{
		mMotorArray[i].setValues(true, 0);
		mLampeArray[i].setValues(0);
	}

  mSHM->commonStart=false;
  mSHM->commonPause=false;
  //mSHM->commonStopp=false;
  mSHM->running=false;
  
	Serial.println("Ausgaenge zurueckgesetzt.");
	
	return false;
}
