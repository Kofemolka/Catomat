#pragma once

class Stepper
{
public:
	Stepper(int in1, int in2, int in3, int in4) :		
		in1(in1),
		in2(in2),
		in3(in3),
		in4(in4)
	{		
	}

	void Setup()
	{
		pinMode(in1, OUTPUT);
		pinMode(in2, OUTPUT);
		pinMode(in3, OUTPUT);
		pinMode(in4, OUTPUT);
	}

	void Spin(float spins, bool clockwise)
	{
		int x = 0;
		for (x = 0; x < Steps/6; x++)
		{
			step(!clockwise);
			delayMicroseconds(StepDelay);
		}		

		for (x = 0; x < spins * (Steps + Steps / 6) / 2; x++)
		{
			step(clockwise);
			delayMicroseconds(StepDelay);
		}		

		for (x = 0; x < Steps / 6; x++)
		{
			step(!clockwise);
			delayMicroseconds(StepDelay);
		}		

		for (x = 0; x < spins * (Steps + Steps / 6) / 2; x++)
		{
			step(clockwise);
			delayMicroseconds(StepDelay);
		}		
	}

private:
	void step(bool clockwise)
	{		
		switch (curStep)
		{
		case 0:
			digitalWrite(in1, LOW);
			digitalWrite(in2, LOW);
			digitalWrite(in3, LOW);
			digitalWrite(in4, HIGH);
			break;
		case 1:
			digitalWrite(in1, LOW);
			digitalWrite(in2, LOW);
			digitalWrite(in3, HIGH);
			digitalWrite(in4, HIGH);
			break;
		case 2:
			digitalWrite(in1, LOW);
			digitalWrite(in2, LOW);
			digitalWrite(in3, HIGH);
			digitalWrite(in4, LOW);
			break;
		case 3:
			digitalWrite(in1, LOW);
			digitalWrite(in2, HIGH);
			digitalWrite(in3, HIGH);
			digitalWrite(in4, LOW);
			break;
		case 4:
			digitalWrite(in1, LOW);
			digitalWrite(in2, HIGH);
			digitalWrite(in3, LOW);
			digitalWrite(in4, LOW);
			break;
		case 5:
			digitalWrite(in1, HIGH);
			digitalWrite(in2, HIGH);
			digitalWrite(in3, LOW);
			digitalWrite(in4, LOW);
			break;
		case 6:
			digitalWrite(in1, HIGH);
			digitalWrite(in2, LOW);
			digitalWrite(in3, LOW);
			digitalWrite(in4, LOW);
			break;
		case 7:
			digitalWrite(in1, HIGH);
			digitalWrite(in2, LOW);
			digitalWrite(in3, LOW);
			digitalWrite(in4, HIGH);
			break;
		default:
			digitalWrite(in1, LOW);
			digitalWrite(in2, LOW);
			digitalWrite(in3, LOW);
			digitalWrite(in4, LOW);
			break;
		}

		if (!clockwise)
		{
			curStep++;
			if (curStep == 8)
			{
				curStep = 0;
			}
		}
		else
		{
			curStep--;
			if (curStep == -1)
			{
				curStep = 7;
			}
		}
	}

private:
	const int in1;
	const int in2;
	const int in3;
	const int in4;

	const int Steps = 64 * 64;
	const int StepDelay = 1000;

	int curStep = 0;
};


