#pragma once

#include "StepperMotor.h"
#include "Mem.h"

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

	void Feed()
	{
		stepper.Spin( ((float)(Mem::GetFoodAmount())) / GrPerRev, true);
	}

private:
	StepperMotor stepper;
		
	const float GrPerRev = 5;
};

