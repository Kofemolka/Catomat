#ifndef SONAR_H
#define SONAR_H

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
		//LOG("Sonar: Setup");
		lastCheck = 0;
		state = SonarState::Idle;
	}

	bool WasVisit()
	{
		return state == SonarState::VisitRecorded;
	}

	int VisitDuration()
	{
		return visitDuration / 1000;
	}

	void Reset()
	{
		//LOG("Sonar Reset");
		state = SonarState::Idle;
	}

	void Check()
	{
		if (state == SonarState::VisitRecorded)
			return;

		if ((millis() - lastCheck) < checkDelay)
			return;

		lastCheck = millis();

		int dist = ping();
		//LOG(dist);

		bool inRange = abs(dist)  < etaDist;

		switch (state)
		{
		case SonarState::Idle:
			if (inRange)
			{
				//LOG("SonarState::InRange");
				state = SonarState::InRange;
				visitBegin = millis();
			}
			break;

		case SonarState::InRange:
			if (inRange)
			{
				if (millis() - visitBegin > trigerDelay)
				{
					//LOG("SonarState::Recording");
					state = SonarState::Recording;
				}
			}
			else
			{
				//LOG("SonarState::Idle");
				state = SonarState::Idle;
			}
			break;

		case SonarState::Recording:
			if (inRange)
			{
				if (millis() - visitBegin > visitConfirmedDelay)
				{
					//LOG("SonarState::Visited");
					state = SonarState::Visited;
				}
			}
			else
			{
				//LOG("SonarState::Reset");
				state = SonarState::Wait;
				resetBegin = millis();
			}
			break;

		case SonarState::Wait:
			if (inRange)
			{
				//LOG("SonarState::Recording");
				state = SonarState::Recording;
			}
			else
			{
				if (millis() - resetBegin > resetDelay)
				{
					//LOG("SonarState::Idle");
					state = SonarState::Idle;
				}
			}
			break;

		case SonarState::Visited:
			if (!inRange)
			{
				//LOG("SonarState::VisitRecorded");
				state = SonarState::VisitRecorded;
				visitDuration = millis() - visitBegin;
			}
		}
	}

private:
	enum SonarState
	{
		Idle,
		InRange,
		Recording,
		Wait,
		Visited,
		VisitRecorded
	};

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

	const int etaDist = 20; //cm

	const int trigerDelay = 3 * 1000;
	const int visitConfirmedDelay = 6 * 1000;
	const int resetDelay = 5 * 1000;

	unsigned long visitBegin = 0;
	unsigned long resetBegin = 0;

	unsigned long visitDuration = 0;

	SonarState state = Idle;
};

#endif
