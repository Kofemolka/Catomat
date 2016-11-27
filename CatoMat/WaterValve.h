#pragma once

#include <Servo.h>
#include "Mem.h"

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

	void Serve()
	{
		servo.attach(pin);
		open();
		servo.detach();

		int pause = ((float)Mem::GetWaterAmount() - gPerMovement) / gPerMsec;
		delay(pause);		

		servo.attach(pin);
		close();
		servo.detach();
	}

private:
	void open()
	{				
		for (curPos; curPos >= openedPos; curPos -= 10)
		{
			servo.write(curPos);			
			delay(stepDelay);
		}
	}

	void close()
	{
		for (curPos; curPos <= closedPos; curPos += 10)
		{			
			servo.write(curPos);			
			delay(stepDelay);
		}	

		delay(stepDelay*4);
	}

private:
	Servo servo;

	const int pin;

	const int closedPos = 135;
	const int openedPos = 65;

	const float gPerMovement = 10;
	const float gPerMsec = 0.1;

	const int stepDelay = 50;

	int curPos;
};
