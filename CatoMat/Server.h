#pragma once

#include <SoftwareSerial.h>
#include "log.h"
#include "lib\wifi\ESP8266wifi.h"
#include "State.h"
#include "EAction.h"

class AppServer
{
public:
	AppServer(int rx, int tx, int reset) :
		serial(rx, tx),
		client(serial, serial, reset, Serial)
	{
		
	}

	void Setup()
	{
		serial.begin(9600);
		client.begin();

		client.setTransportToTCP();				
	}
	
	Global::EAction Check(State::EMode currentMode)
	{
		if ((millis() - lastCheck) < checkDelay)
			return Global::EAction::None;
		
		lastCheck = millis();

		Global::EAction action = Global::EAction::None;

		if (connect())
		{
			postStatus(currentMode);

			disconnect();
		}			

		if (connect())
		{
			String cmd = getCommand();			

			if (cmd.indexOf("{feed}") != -1)
			{
				action = Global::EAction::Feed;
			}
			else if (cmd.indexOf("{pump}") != -1)
			{
				action = Global::EAction::Pump;
			}		
			else if (cmd.indexOf("{manual}") != -1)
			{
				action = Global::EAction::ModeManual;
			}
			else if (cmd.indexOf("{auto}") != -1)
			{
				action = Global::EAction::ModeAuto;
			}			

			disconnect();
		}		

		return action;
	}

	void PostUpdate(Global::EAction action)
	{
		if (action != Global::EAction::None && connect())
		{
			String req = "action=update&event=";
			
			if (action == Global::EAction::Feed)
				req += "feed";
			else if (action == Global::EAction::Pump)
				req += "pump";
			else 
				req += "visit";

			sendHttpRequest(actionPOST, req.c_str());

			delay(1000);

			disconnect();
		}
	}	

private:	
	bool connect()
	{
		if (!client.isConnectedToAP())
		{
			if (!client.connectToAP(ssid, pwd))
			{
				LOG("AP Fail");

				return false;
			}
		}			

		if (client.connectToServer(host, "80"))
		{
			return true;
		}		

		LOG("Server Fail");

		return false;
	}

	void disconnect()
	{
		client.disconnectFromServer();
		delay(2000);
	}

	String getCommand()
	{
		sendHttpRequest(actionPOST, "action=query");

		if(waitForResponse(10000, "[action]="))
		{
			char buff[30];
			if (readBuffer(buff, 29, 1000))
			{
				LOG(buff);
			}

			return buff;
		}

		return "";
	}

	void postStatus(State::EMode currentMode)
	{
		String req = "action=update&event=mode&mode=";
		if (currentMode == State::EMode::Auto)
			req += "A";
		else
			req += "M";

		sendHttpRequest(actionPOST, req.c_str());	

		delay(5000);
	}

	void sendHttpRequest(const char* action, const char* url)
	{		
		char req[MSG_BUFFER_MAX];

		sprintf(req, "%s http://%s/index.php?%s HTTP/1.1\r\n"
					 "Authorization: %s\r\n"
					 "Host: %s\r\n"
					 "Content-Length: 0\r\n"
					 "\r\n", action, host, url, authToken, host);

		LOG(url);		

		client.send(0, req, true);
	}

	bool waitForResponse(int timeout, const char* text1) {
		// setup buffers on stack & copy data from PROGMEM pointers
		char buf1[16] = { '\0' };		
		if (text1 != NULL)
			strcpy(buf1, (char *)text1);
		
		byte len1 = strlen(buf1);
		byte pos1 = 0;

		// read chars until first match or timeout
		unsigned long stop = millis() + timeout;
		do {
			while (serial.available()) {
				char c = serial.read();
				Serial.write(c);
				pos1 = (c == buf1[pos1]) ? pos1 + 1 : 0;				
				if (len1 > 0 && pos1 == len1)
					return true;				
			}
			delay(10);
		} while (millis() < stop);

		return false;
	}

	byte readBuffer(char* buf, byte count, int timeout) {
		
		byte pos = 0;
		unsigned long stop = millis() + timeout;
		do {

		while (serial.available() && pos < count) {
			
			buf[pos++] = serial.read();
		}
		delay(10);
		} while (millis() < stop);

		buf[pos] = '\0';
		return pos;
	}

	ESP8266wifi client;
	SoftwareSerial serial;

	const char* ssid = "";
	const char* pwd = "";
	const char* host = "catomat.com.ua";
	const char* authToken = "Basic ";
	const char* actionGET = "GET";
	const char* actionPOST = "POST";	

	const unsigned long checkDelay = 1000 * 30;
	unsigned long lastCheck = 0;
};