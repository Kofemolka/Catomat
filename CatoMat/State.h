#pragma once

#include "log.h"
#include "Mem.h"

class State
{
public:
	State(int pinBtn, int pinLedAuto, int pinLedManual) :
		pinBtn(pinBtn),
		pinLedAuto(pinLedAuto),
		pinLedManual(pinLedManual)
	{

	}

	enum EFlash
	{
		None,
		Fast,
		Slow
	};

	enum EMode : uint8_t
	{
		Manual = 0,
		Auto = 1
	};

	void Setup()
	{
		LOG("State: Setup");

		pinMode(pinLedAuto, OUTPUT);
		pinMode(pinLedManual, OUTPUT);
		pinMode(pinBtn, INPUT);

		currentMode = (EMode)Mem::GetMode();
	}

	virtual void Check()
	{
		unsigned long now = millis();

		if (digitalRead(pinBtn) == HIGH)
		{
			if (btnPress)
			{
				if ((now - pressStart) >= switchDelay)
				{
					if (Current() == EMode::Auto)
					{
						Switch(EMode::Manual);
					}
					else
					{
						Switch(EMode::Auto);
					}

					btnPress = false;
				}
			}
			else
			{
				btnPress = true;
				pressStart = now;
			}
		}
		else
		{
			if (btnPress && (now - pressStart) >= feedDelay)
			{
				needFood = true;
			}

			btnPress = false;
		}		
	}

	EMode Current()
	{
		return currentMode;
	}

	bool NeedFood()
	{
		bool res = needFood;
		needFood = false;
		return res;
	}

	void Switch(EMode newMode)
	{
		currentMode = newMode;

		Mem::SetMode(currentMode);
				
		if (currentMode == EMode::Auto)
		{
			LOG("Mode: Auto");
		}
		else
		{
			LOG("Mode: Manual");
		}
	}

	void Flash(EFlash type)
	{
		flashing = type;
	}

	void UpdateLeds()
	{
		if (Current() == EMode::Auto)
		{
			flashLed(pinLedAuto);
			digitalWrite(pinLedManual, LOW);
		}
		else
		{
			digitalWrite(pinLedAuto, LOW);
			flashLed(pinLedManual);
		}
	}

private:
	void flashLed(int pin)
	{
		if (flashing != EFlash::None)
		{
			int delay = flashing == EFlash::Fast ? flashDelayFast : flashDelaySlow;
			if ((millis() - flashSwitchTime) >= delay)
			{
				flashHigh = !flashHigh;
				flashSwitchTime = millis();
			}

			digitalWrite(pin, flashHigh ? HIGH : LOW);
		}
		else
		{
			digitalWrite(pin, HIGH);
		}
	}

	const int pinBtn;
	const int pinLedAuto;
	const int pinLedManual;

	const unsigned long switchDelay = 1000 * 3;
	const unsigned long feedDelay = 1000;
	
	bool btnPress = false;
	unsigned long pressStart = 0;
	
	EFlash flashing = EFlash::None;
	bool flashHigh = false;
	unsigned long flashSwitchTime = 0;
	const unsigned long flashDelaySlow = 400;
	const unsigned long flashDelayFast = 200;

	EMode currentMode;
	bool needFood = false;
};
