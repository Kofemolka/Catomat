#ifndef STEPPERMOTOR_H
#define STEPPERMOTOR_H

class StepperMotor
{
public:
	StepperMotor(int en, int clk, int cw) :
		_en(en), _clk(clk), _cw(cw)
	{
	}

	void Setup()
	{
		pinMode(_en, OUTPUT);
		pinMode(_clk, OUTPUT);
		pinMode(_cw, OUTPUT);

		digitalWrite(_en, HIGH);
	}

	void Spin(float spins, bool clockwise)
	{
		digitalWrite(_en, LOW);

		digitalWrite(_cw, clockwise ? HIGH : LOW);

		for (int s = 0; s < spins*StepsPerRev; s++)
		{
			step();
		}

		digitalWrite(_en, HIGH);
	}

private:
	void step()
	{
		digitalWrite(_clk, LOW);
		delayMicroseconds(StepDelay);
		digitalWrite(_clk, HIGH);
		delayMicroseconds(StepDelay);
	}

	const int _en;
	const int _clk;
	const int _cw;

	const int StepsPerRev = 200*16;
	const int StepDelay = 200;
};

#endif
