#pragma once

#include <SoftwareSerial.h>
#include "log.h"
#include "State.h"
#include "EAction.h"
#include "Mem.h"

class AppServer
{
public:
	AppServer(int rx, int tx, int reset) :
		serial(rx, tx),
		_resetPin(reset)
	{
		pinMode(_resetPin, OUTPUT);
	}

	void Setup()
	{
		LOG("AppServer: Setup");

		serial.begin(9600);

		ResetEsp();
	}
	
	Global::EAction Check(State::EMode currentMode)
	{		
		Global::EAction action = Global::EAction::None;
			
		String cmd = getCommand();			

		if (cmd.indexOf("feed") != -1)
		{
			action = Global::EAction::Feed;
		}
		else if (cmd.indexOf("pump") != -1)
		{
			action = Global::EAction::Pump;
		}		
		else if (cmd.indexOf("manual") != -1)
		{
			action = Global::EAction::ModeManual;
		}
		else if (cmd.indexOf("auto") != -1)
		{
			action = Global::EAction::ModeAuto;
		}	
		else if (cmd.indexOf("food=") != -1)
		{
			String s = cmd.substring(cmd.indexOf("food=") + 5);
			int amount = s.toInt();
			if (amount > 0)
			{
				Mem::SetFoodAmount(amount);
			}
		}
		else if (cmd.indexOf("water=") != -1)
		{
			String s = cmd.substring(cmd.indexOf("water=") + 6);
			int amount = s.toInt();
			if (amount > 0)
			{
				Mem::SetWaterAmount(amount);
			}
		}

		if ((millis() - lastCheck) > checkDelay)
		{
			postStatus(currentMode);
			lastCheck = millis();
			delay(50);
		}

		return action;
	}

	void PostUpdate(Global::EAction action)
	{		
		switch (action)
		{
		case Global::EAction::Feed:
			serial.println("feed");
			break;

		case Global::EAction::Pump:
			serial.println("pump");
			break;

		case Global::EAction::Visit:
			serial.println("visit");
			break;		
		}	
	}	

private:
	String getCommand()
	{
		char buff[256];
		int ndx = 0;

		if (serial.available() > 0)
		{
			String cmd = serial.readString();
			LOG(cmd.c_str());

			int n = cmd.indexOf("[inbox]:");
			if (n != -1)
			{
				return cmd.substring(n + 1);
			}
		}

		return "";
	}

	void postStatus(State::EMode currentMode)
	{
		LOG("AppServer: postStatus");

		String mode = "mode: ";
		if (currentMode == State::EMode::Auto)
			mode += "auto";
		else
			mode += "manual";

		serial.println(mode);
	}

	void ResetEsp()
	{		
		LOG("AppServer: Reset ESP");
		digitalWrite(_resetPin, LOW);
		delay(100);
		digitalWrite(_resetPin, HIGH);
	}
		
	SoftwareSerial serial;
	int _resetPin;
	
	const unsigned long checkDelay = 1000 * 60;
	unsigned long lastCheck = 0;
};