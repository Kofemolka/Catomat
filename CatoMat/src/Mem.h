#ifndef MEM_H
#define MEM_H

#include <EEPROM.h>
#include "log.h"

class Mem
{
public:
	static uint8_t GetMode()
	{
		return readVal(modeOff);
	}

	static void SetMode(uint8_t mode)
	{
		writeVal(modeOff, mode);

		//LOG(String("Mem:SetMode:") + mode);
	}

	static uint8_t GetFoodAmount()
	{
		return readVal(foodOff);
	}

	static void SetFoodAmount(uint8_t amount)
	{
		writeVal(foodOff, amount);

		//LOG(String("Mem:SetFoodAmount:") + amount);
	}

	static uint8_t GetWaterAmount()
	{
		return readVal(waterOff);
	}

	static void SetWaterAmount(uint8_t amount)
	{
		writeVal(waterOff, amount);

		//LOG(String("Mem:SetWaterAmount:") + amount);
	}

private:
	static void writeVal(const unsigned long off, uint8_t val)
	{
		if (val > 255)
		{
			val = 255;
		}

		EEPROM.update(off, val);
	}

	static uint8_t readVal(const unsigned long off)
	{
		return EEPROM.read(off);
	}

	static const unsigned int modeOff = 0;
	static const unsigned int foodOff = 1;
	static const unsigned int waterOff = 2;
};

#endif
