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
			serial.println("outbox/ack:manual");
		}
		else if (cmd.indexOf("inbox/mode:auto") != -1)
		{
			action = Global::EAction::ModeAuto;
			serial.println("outbox/ack:auto");
		}	
		else if (cmd.indexOf("inbox/adj/food:") != -1)
		{
			String s = cmd.substring(cmd.indexOf(":") + 1);
			int amount = s.toInt();
			if (amount > 0)
			{
				Mem::SetFoodAmount(amount);
				serial.println("outbox/ack:food");
			}
		}
		else if (cmd.indexOf("inbox/adj/water:") != -1)
		{
			String s = cmd.substring(cmd.indexOf(":") + 1);
			int amount = s.toInt();
			if (amount > 0)
			{
				Mem::SetWaterAmount(amount);
				serial.println("outbox/ack:water");
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

		delay(1000);
	}	

private:
	String _cmd;

	String getCommand()
	{
		while (serial.available() > 0)
		{
			char c = (char)serial.read();

			_cmd += c;
			
			if (c == '\n' || c == '\r')
			{
				String cmd = _cmd;
				_cmd = "";
				
				if (cmd.length() > 0)
				{					
					LOG(cmd);

					return cmd;
				}
			}
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

		delay(1000);

		serial.print("outbox/uptime:");
		serial.println(millis() / 1000 / 60);

		delay(1000);
	}

	void ResetEsp()
	{		
		LOG("AppServer: Reset ESP");

		digitalWrite(_resetPin, LOW);
		delay(1000);
		digitalWrite(_resetPin, HIGH);
		delay(1000);		
	}
		
	SoftwareSerial serial;
	int _resetPin;
	
	const unsigned long checkDelay = 1000 * 60;
	unsigned long lastCheck = 0;	
};