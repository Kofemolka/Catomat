#pragma once

#include "EAction.h"

class Schedule
{
public:
	Schedule(unsigned long foodInterval, unsigned long waterInterval)
		: foodInterval(foodInterval), waterInterval(waterInterval)
	{
		lastFeed = lastPump = millis();		
	}

	Global::EAction Check()
	{
		unsigned long now = millis();

		int action = Global::EAction::None;

		if ((now - lastFeed) >= foodInterval)
		{
			lastFeed = now;
			return Global::EAction::Feed;
		}

		if ((now - lastPump) >= waterInterval)
		{
			lastPump = now;
			return Global::EAction::Pump;
		}			

		return Global::EAction::None;
	}
	
private:
	unsigned long lastFeed;
	unsigned long lastPump;

	const unsigned long foodInterval;
	const unsigned long waterInterval;
};

