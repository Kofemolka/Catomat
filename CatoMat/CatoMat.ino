/*
 Name:		CatoMat.ino
 Created:	3/23/2016 10:50:46 AM
 Author:	Andriy_Yakuba
*/

#include <Arduino.h>
#include <EEPROM.h>
#include <Servo.h>

#include "log.h"
#include "WaterValve.h"
#include "Feeder.h"
#include "Schedule.h"
#include "State.h"
#include "Server.h"
#include "Sonar.h"
#include "EAction.h"

const unsigned long SECOND = 1000;
const unsigned long MINUTE = SECOND * 60;
const unsigned long HOUR = MINUTE * 60;

//Schedule schedule(MINUTE/3, MINUTE/2);
Schedule schedule(HOUR * 4, HOUR * 24);

State state(10, 9, 8);
WaterValve waterValve(11);
Feeder feeder(7, 5, 6);
Sonar sonar(12, 12);
AppServer server(4, 3, 2);

#define FOOD_AMOUNT 15
#define WATER_AMOUNT 100

void setup() 
{
	OCR0A = 0xAF;
	TIMSK0 |= _BV(OCIE0A);

	Serial.begin(9600);

	state.Setup();
	state.Flash(State::EFlash::Fast);
	
	feeder.Setup();
	waterValve.Setup();	
	sonar.Setup();
	server.Setup();

	state.Flash(State::EFlash::None);
}

void loop()
{
	Global::EAction action = server.Check(state.Current());
	
	if (action == Global::EAction::None && state.Current() == State::EMode::Auto)
	{
		action = schedule.Check();
	}		

	switch (action)
	{	
	case Global::Feed:
		Feed(true);
		break;

	case Global::Pump:
		Pour();
		break;
	
	case Global::ModeManual:
		state.Switch(State::EMode::Manual);
		break;

	case Global::ModeAuto:
		state.Switch(State::EMode::Auto);
		break;	
	}
	
	if (sonar.WasVisit())
	{
		server.PostUpdate(Global::EAction::Visit);
		sonar.Reset();
	}	
}

void Feed(bool update)
{	
	LOG("Eat");
		
	state.Flash(State::EFlash::Slow);
		
	feeder.Feed(FOOD_AMOUNT);

	if (update)
	{
		server.PostUpdate(Global::EAction::Feed);
	}

	state.Flash(State::EFlash::None);
}

void Pour()
{
	LOG("Drink");

	state.Flash(State::EFlash::Slow);

	waterValve.Serve(WATER_AMOUNT);

	server.PostUpdate(Global::EAction::Pump);
	
	state.Flash(State::EFlash::None);
}

SIGNAL(TIMER0_COMPA_vect)
{
	unsigned long currentMillis = millis();

	state.Check();
	sonar.Check();

	if(state.NeedFood())
	{
		Feed(false);
	}
}

