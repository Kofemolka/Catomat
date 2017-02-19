/*
 Name:		CatoMat.ino
 Created:	3/23/2016 10:50:46 AM
 Author:	Andriy_Yakuba
*/

#include <Arduino.h>

#include "log.h"
#include "WaterValve.h"
#include "Feeder.h"
#include "Schedule.h"
#include "State.h"
#include "AppServer.h"
#include "Sonar.h"
#include "EAction.h"

const unsigned long SECOND = 1000;
const unsigned long MINUTE = SECOND * 60;
const unsigned long HOUR = MINUTE * 60;

Schedule schedule(HOUR * 4, HOUR * 24);
State state(10, 9, 8, &onStateChanged);
WaterValve waterValve(11);
Feeder feeder(7, 5, 6);
Sonar sonar(12, 13);
AppServer server(3);


MsgBuffer AppServer::_inMsgBuf;
MsgBuffer AppServer::_outMsgBuf;
volatile Global::EAction AppServer::_serverAction;

void onStateChanged(EMode mode)
{
	if (mode == EMode::Auto)
	{
		schedule.Reset();
	}
}

void setup()
{
	Serial.begin(9600);

	setupInterrupts();

	LOG(F("CatoMat: Setup..."));

	state.Setup();
	state.Flash(State::EFlash::Fast);

	feeder.Setup();
	waterValve.Setup();
	sonar.Setup();
	server.Setup();

	state.Flash(State::EFlash::None);

	LOG(F("CatoMat: Ready"));
}

void setupInterrupts()
{
	cli();

	TCCR2A = 0;// set entire TCCR2A register to 0
	TCCR2B = 0;// same for TCCR2B
	TCNT2 = 0;//initialize counter value to 0
			  // set compare match register for 8khz increments
	OCR2A = 249;// = (16*10^6) / (8000*8) - 1 (must be <256)
				// turn on CTC mode
	TCCR2A |= (1 << WGM21);
	// Set CS21 bit for 8 prescaler
	TCCR2B |= (1 << CS21);
	// enable timer compare interrupt
	TIMSK2 |= (1 << OCIE2A);

	sei();
}

void loop()
{
	Global::EAction action = Global::EAction::None;

	state.Check();

	if (state.NeedFood())
	{
		action = Global::Feed;
	}
	else
	{
		action = server.Check(state.Current());
	}

	if (action == Global::EAction::None && state.Current() == EMode::Auto)
	{
		action = schedule.Check();
	}

	switch (action)
	{
	case Global::Feed:
		Feed();
		break;

	case Global::Pump:
		Pour();
		break;

	case Global::ModeManual:
		state.Switch(EMode::Manual);
		break;

	case Global::ModeAuto:
		state.Switch(EMode::Auto);
		break;

	default:
		break;
	}

	sonar.Check();

	if (sonar.WasVisit())
	{
		server.PostUpdate(Global::EAction::Visit, sonar.VisitDuration());
		sonar.Reset();
	}
}

void Feed()
{
	LOG(F("Serving Food..."));

	state.Flash(State::EFlash::Slow);

	feeder.Feed();

	server.PostUpdate(Global::EAction::Feed);

	state.Flash(State::EFlash::None);
}

void Pour()
{
	LOG(F("Serving Water..."));

	state.Flash(State::EFlash::Slow);

	waterValve.Serve();

	server.PostUpdate(Global::EAction::Pump);

	state.Flash(State::EFlash::None);
}

ISR(TIMER2_COMPA_vect)
{
	state.UpdateLeds();
}
