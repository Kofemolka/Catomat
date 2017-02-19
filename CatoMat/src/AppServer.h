#ifndef APPSERVER_H
#define APPSERVER_H

#include "log.h"
#include "State.h"
#include "EAction.h"
#include "Mem.h"
#include "MsgBuffer.h"
#include <Wire.h>

#define C_MODE 1
#define C_VISIT 2
#define C_ACK 3
#define C_FOOD_AMOUNT 4
#define C_WATER_AMOUNT 5
#define D_ACK_FEED 1
#define D_ACK_PUMP 2
#define D_ACK_AUTO 3
#define D_ACK_MANUAL 4
#define D_ACK_ADJ_FOOD 5
#define D_ACK_ADJ_WATER 6

class AppServer
{
public:
	AppServer(int reset) :
		_resetPin(reset)
	{
		pinMode(_resetPin, OUTPUT);

	}

	void Setup()
	{
		//LOG(F("AppServer: Setup"));
		_inMsgBuf.ID = "IN";
		_outMsgBuf.ID = "OUT";

		ResetEsp();

		Wire.begin(9);
		Wire.onRequest(this->requestEvent);
		Wire.onReceive(receiveEvent);

		lastCheck = millis();
	}

	Global::EAction Check(EMode currentMode)
	{
		processMsg();

		Global::EAction action = _serverAction;
		_serverAction = Global::EAction::None;

		if ((millis() - lastCheck) > checkDelay)
		{
			postStatus(currentMode);
			lastCheck = millis();
			delay(50);
		}

		return action;
	}

	void PostUpdate(Global::EAction action, byte value = 0)
	{
		switch (action)
		{
		case Global::EAction::Feed:
			sendMsg(C_ACK, D_ACK_FEED);
			break;

		case Global::EAction::Pump:
			sendMsg(C_ACK, D_ACK_PUMP);
			break;

		case Global::EAction::Visit:
			sendMsg(C_VISIT, value);
			break;
		}
	}

private:
	static void receiveEvent(int countToRead) {
		byte data[MsgBuffer::MSG_SIZE] = { 0 };

		//LOG(String("receiveEvent: ") + countToRead);
		int i = 0;
		while (Wire.available()) {
			byte b = Wire.read();
			if (i<MsgBuffer::MSG_SIZE)
			{
				data[i++] = b;
			}
		}

		_inMsgBuf.Put(data);
	}

	static void requestEvent() {
		//LOG("requestEvent()");
		byte msg[MsgBuffer::MSG_SIZE] = { 0 };
		if(_outMsgBuf.Pop(msg))
		{
				//LOG("requestEvent() - POP");
				for(int i=0;i<MsgBuffer::MSG_SIZE;i++)
				{
						Wire.write(msg[i]);
				}
		}
		else
		{
			//LOG("requestEvent() - NULL");
			for (int i = 0; i < MsgBuffer::MSG_SIZE; i++)
			{
				Wire.write((byte)0);
			}
		}
	}

	void processMsg()
	{
		_serverAction = Global::EAction::None;

		byte msg[MsgBuffer::MSG_SIZE] = {0};
		if (!_inMsgBuf.Pop(msg))
		{
			return; //nothing
		}

		if (msg[0] != msg[1] || msg[0] != msg[2] ||
			msg[3] != msg[4] || msg[3] != msg[5])
		{
			LOG(F("AppServer: Invalid message"));
			return;
		}

		switch (msg[0]) {
			case 1:
				if (msg[3] == 1)
				{
					_serverAction = Global::EAction::ModeAuto;
					sendMsg(C_ACK, D_ACK_AUTO);
				}
				else if (msg[3] == 2)
				{
					_serverAction = Global::EAction::ModeManual;
					sendMsg(C_ACK, D_ACK_MANUAL);
				}
				break;
			case 2:
				if (msg[3] == 1)
				{
					_serverAction = Global::EAction::Feed;
					//sendMsg(C_ACK, D_ACK_FEED);
				}
				else if (msg[3] == 2)
				{
					_serverAction = Global::EAction::Pump;
					//sendMsg(C_ACK, D_ACK_PUMP);
				}
				break;
			case 3:
				Mem::SetFoodAmount(msg[3]);
				sendMsg(C_ACK, D_ACK_ADJ_FOOD);
				break;
			case 4:
				Mem::SetWaterAmount(msg[3]);
				sendMsg(C_ACK, D_ACK_ADJ_WATER);
				break;
		}
	}

	static void sendMsg(byte code, byte data)
	{
		byte msg[MsgBuffer::MSG_SIZE];

		msg[0] = msg[1] = msg[2] = code;
		msg[3] = msg[4] = msg[5] = data;

		_outMsgBuf.Put(msg);
	}

	void postStatus(EMode currentMode)
	{
		//LOG(F("AppServer: postStatus"));

		if (currentMode == EMode::Auto)
			sendMsg(C_MODE, 1);
		else
			sendMsg(C_MODE, 2);

		sendMsg(C_FOOD_AMOUNT, Mem::GetFoodAmount());
		sendMsg(C_WATER_AMOUNT, Mem::GetWaterAmount());
	}

	void ResetEsp()
	{
	//	LOG(F("AppServer: Reset ESP"));

		digitalWrite(_resetPin, LOW);
		delay(1000);
		digitalWrite(_resetPin, HIGH);
		delay(1000);
	}

	int _resetPin;

	const unsigned long checkDelay = 60000;
	unsigned long lastCheck = 0;

	static volatile Global::EAction _serverAction;

	static MsgBuffer _outMsgBuf;
	static MsgBuffer _inMsgBuf;
};

#endif
