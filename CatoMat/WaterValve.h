#pragma once

#include <Servo.h>

class WaterValve
{
public:
	WaterValve(int pin) :		
		pin(pin)
	{
		curPos = closedPos;
	}
	
	void Setup()
	{			
		LOG("WaterValve: Setup");

		servo.attach(pin);

		servo.write(closedPos);
		delay(300);

		servo.detach();
	}

	void Serve(int grams)
	{
		servo.attach(pin);
		open();
		servo.detach();

		int pause = ((float)grams - gPerMovement) / gPerMsec;
		delay(pause);		

		servo.attach(pin);
		close();
		servo.detach();
	}

private:
	void open()
	{		
		for (curPos; curPos >= openedPos; curPos -= 5)
		{
			servo.write(curPos);			
			delay(stepDelay);
		}
	}

	void close()
	{
		for (curPos; curPos <= closedPos; curPos += 5)
		{			
			servo.write(curPos);			
			delay(stepDelay);
		}	
	}

private:
	Servo servo;

	const int pin;

	const int closedPos = 140;
	const int openedPos = 70;

	const float gPerMovement = 3;
	const float gPerMsec = 0.022;

	const int stepDelay = 50;

	int curPos;
};
