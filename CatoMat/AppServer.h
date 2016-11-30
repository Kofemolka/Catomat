#pragma once

#include <SoftwareSerial.h>
#include "log.h"
#include "State.h"
#include "EAction.h"
#include "Mem.h"
#include "SerialCommand.h"

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

#define OUTBOX_MODE "outbox/mode"
#define OUTBOX_UPTIME "outbox/uptime"
#define OUTBOX_VISIT "outbox/visit"
#define OUTBOX_ACK "outbox/ack"

class AppServer
{
public:
	AppServer(int rx, int tx, int reset) :
		serial(rx, tx),
		serialCmd(serial),
		_resetPin(reset)
	{
		pinMode(_resetPin, OUTPUT);
	}

	void Setup()
	{
		LOG("AppServer: Setup");

		serial.begin(9600);

		ResetEsp();

		lastCheck = millis();
	}
	
	Global::EAction Check(State::EMode currentMode)
	{		
		Global::EAction action = Global::EAction::None;
			
		String cmd = serialCmd.GetCommand();	
		if (cmd.length() > 0 && cmd[0] != '\n' && cmd[0] != '\r')
		{
			LOG(cmd);
		}

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

			sendMsg(OUTBOX_ACK, "manual");
		}
		else if (cmd.indexOf("inbox/mode:auto") != -1)
		{
			action = Global::EAction::ModeAuto;

			sendMsg(OUTBOX_ACK, "auto");			
		}	
		else if (cmd.indexOf("inbox/adj/food:") != -1)
		{
			String s = cmd.substring(cmd.indexOf(":") + 1);
			int amount = s.toInt();
			if (amount > 0)
			{
				Mem::SetFoodAmount(amount);
				
				sendMsg(OUTBOX_ACK, "food");
			}
		}
		else if (cmd.indexOf("inbox/adj/water:") != -1)
		{
			String s = cmd.substring(cmd.indexOf(":") + 1);
			int amount = s.toInt();
			if (amount > 0)
			{
				Mem::SetWaterAmount(amount);

				sendMsg(OUTBOX_ACK, "water");				
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
			sendMsg(OUTBOX_ACK, "feed");			
			break;

		case Global::EAction::Pump:
			sendMsg(OUTBOX_ACK, "pump");			
			break;

		case Global::EAction::Visit:
			sendMsg(OUTBOX_VISIT, String(value).c_str());			
			break;		
		}	
	}	

private:
	void sendMsg(const char* topic, const char* msg)
	{
		serial.print(topic);
		serial.print(":");
		serial.println(msg);

		delay(100);
	}

	void postStatus(State::EMode currentMode)
	{
		LOG("AppServer: postStatus");

		if (currentMode == State::EMode::Auto)
			sendMsg(OUTBOX_MODE, "auto");
		else
			sendMsg(OUTBOX_MODE, "manual");	

		sendMsg(OUTBOX_UPTIME, String(millis() / 1000 / 60).c_str());	
	}

	void ResetEsp()
	{		
		LOG("AppServer: Reset ESP");

		digitalWrite(_resetPin, LOW);
		delay(1000);
		digitalWrite(_resetPin, HIGH);
		delay(1000);		
	}
		
	SerialCommand serialCmd;
	SoftwareSerial serial;
	int _resetPin;
	
	const unsigned long checkDelay = 60000;
	unsigned long lastCheck = 0;	
};