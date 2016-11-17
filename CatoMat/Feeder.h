#pragma once

#include "StepperMotor.h"

class Feeder
{
public:
	Feeder(int en, int clk, int cw) :
		stepper(en, clk, cw) {}
		
	void Setup()
	{
		LOG("Feeder: Setup");

		stepper.Setup();
	}

	void Feed(int grams)
	{
		stepper.Spin( ((float)(grams)) / GrPerRev, true);
	}

private:
	StepperMotor stepper;

	const float GrPerRev = 5;
};

