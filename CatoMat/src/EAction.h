#ifndef EACTION_H
#define EACTION_H

namespace Global
{
	enum EAction
	{
		None		= 0,
		Feed		= 1,
		Pump		= 2,
		Visit		= 4,
		ModeManual	= 8,
		ModeAuto	= 16
	};
}

#endif
