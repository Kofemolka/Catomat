#pragma once

#include <SoftwareSerial.h>
#include "log.h"
#include "State.h"
#include "EAction.h"
#include "Mem.h"

//inbox/
// mode: auto / manual
//	cmd: feed / pump
//	adj/
//	 food: val
//	 water: val
//
//outbox/
// mode: auto / manual
// uptime: val
// visit: val
// ack: feed / pump

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

		if (cmd.indexOf("inbox/cmd:feed") != -1)
		{
			action = Global::EAction::Feed;
		}
		else if (cmd.indexOf("inbox/cmd:pump") != -1)
		{
			action = Global::EAction::Pump;
		}		
		else if (cmd.indexOf("inbox/mode:manual") != -1)
		{
			action = Global::EAction::ModeManual;
		}
		else if (cmd.indexOf("inbox/mode:auto") != -1)
		{
			action = Global::EAction::ModeAuto;
		}	
		else if (cmd.indexOf("inbox/adj/food:") != -1)
		{
			String s = cmd.substring(cmd.indexOf(":") + 1);
			int amount = s.toInt();
			if (amount > 0)
			{
				Mem::SetFoodAmount(amount);
			}
		}
		else if (cmd.indexOf("inbox/adj/water:") != -1)
		{
			String s = cmd.substring(cmd.indexOf(":") + 1);
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

	void PostUpdate(Global::EAction action, int value = 0)
	{		
		switch (action)
		{
		case Global::EAction::Feed:
			serial.println("outbox/ack:feed");
			break;

		case Global::EAction::Pump:
			serial.println("outbox/ack:pump");
			break;

		case Global::EAction::Visit:
			serial.print("outbox/visit:");
			serial.println(value);
			break;		
		}	
	}	

private:
	String getCommand()
	{
		if (serial.available() > 0)
		{
			String cmd = serial.readString();
			LOG(cmd.c_str());

			return cmd;
		}

		return "";
	}

	void postStatus(State::EMode currentMode)
	{
		LOG("AppServer: postStatus");

		serial.print("outbox/mode:");
		if (currentMode == State::EMode::Auto)
			serial.println("auto");
		else
			serial.println("manual");

		serial.print("outbox/uptime:");
		serial.println(millis() / 1000);
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