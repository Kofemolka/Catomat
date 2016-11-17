#pragma once
#include "log.h"

#define abs(x) ((x)<0 ? -(x) : (x))

class Sonar
{
public:
	Sonar(int trigPin, int echoPin) :		
		trigPin(trigPin), echoPin(echoPin)
	{

	}

	void Setup()
	{
		LOG("Sonar: Setup");
	}

	bool WasVisit()
	{
		return wasVisit && visitEnded;
	}

	void Reset()
	{
		LOG("Sonar Reset");
		wasVisit = false;
		visitEnded = false;
	}

	void Check()
	{
		if (WasVisit())
			return;

		if ((millis() - lastCheck) < checkDelay)
			return;

		lastCheck = millis();

		int dist = ping();
		//LOG(dist);		

		bool overflow = abs(dist - etaDist) > etaTresh;

		if (!visitState) //not counting yet
		{
			if (overflow) //out of threshhold - start counting
			{			
				LOG("Sonar: Begin Visit");
				visitState = true;
				visitBegin = millis();

				resetStarted = false;
			}			
		}
		else
		{
			//already counting
			if (overflow) 
			{
				if (resetStarted) //reset reset :)
				{					
					resetStarted = false;
					resetBegin = 0;
				}

				if (millis() - visitBegin > trigerDelay) //mark visit
				{					
					wasVisit = true;					

					return;
				}
			}
			else
			{
				if (wasVisit)
				{
					LOG("Sonar: End Visit");
					visitEnded = true;
					return;
				}

				if (resetStarted) 
				{
					if (millis() - resetBegin > resetDelay)
					{						
						visitState = false;
					}
				}
				else
				{					
					resetStarted = true;
					resetBegin = millis();
				}
			}
		}

	}	

private:
	unsigned long ping()
	{
		pinMode(trigPin, OUTPUT); // Switch signalpin to output
		digitalWrite(trigPin, LOW); // Send low pulse
		delayMicroseconds(2); // Wait for 2 microseconds
		digitalWrite(trigPin, HIGH); // Send high pulse
		delayMicroseconds(5); // Wait for 5 microseconds
		digitalWrite(trigPin, LOW); // Holdoff
		pinMode(echoPin, INPUT); // Switch signalpin to input
		digitalWrite(echoPin, HIGH); // Turn on pullup resistor
											  // please note that pulseIn has a 1sec timeout, which may
											  // not be desirable. Depending on your sensor specs, you
											  // can likely bound the time like this -- marcmerlin
											  // echo = pulseIn(ultraSoundSignal, HIGH, 38000)
		int echo = pulseIn(echoPin, HIGH); //Listen for echo
		int ultrasoundValue = (echo / 58.138); 
		return ultrasoundValue;
	}	

	int trigPin;
	int echoPin;

	const unsigned long checkDelay = 500;
	unsigned long lastCheck = 0;
	
	const int etaDist = 26;
	const int etaTresh = 3;
	const int trigerDelay = 5 * 1000;
	const int resetDelay = 2 * 1000;

	bool visitState = false;
	bool visitEnded = false;
	unsigned long visitBegin = 0;
	bool resetStarted = false;
	unsigned long resetBegin = 0;

	bool wasVisit = false;
};