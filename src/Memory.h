/*
 * Data.h
 *
 *  Created on: Nov 17, 2019
 *      Author: fox
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include <cstdint>
#include <array>

#include "stm32f4xx.h"
#include "Global.h"

class Memory {
public:
	Memory(){}
	virtual ~Memory(){}
public:

	static constexpr uint16_t size = 33;	// Total amount of Memory used for the sensor data.

	std::array<uint8_t, size> mem = {0};	// Memory for sensory data relevant for the system.

	// Sensor data gathered at uc (16 bytes).
	uint16_t &t1 = reinterpret_cast<uint16_t&>(mem[0]);		// #1 MAX6675 temperature value.
	uint16_t &t2 = reinterpret_cast<uint16_t&>(mem[2]);		// #2 MAX6675 temperature value.
	int16_t &t3 = reinterpret_cast<int16_t&>(mem[4]);		// BMP280 temperature value.
	uint32_t &p = reinterpret_cast<uint32_t&>(mem[6]);		// BMP280 pressure value.
	uint16_t &rpm = reinterpret_cast<uint16_t&>(mem[10]);	// Rounds Per Minute (RPM) spinning of the motor.
	uint32_t &h = reinterpret_cast<uint32_t&>(mem[12]);		// Cumulated total time the motor is/has been spinning.

	// Sensor data gathered at smartphone and send to uc via bluetooth.
	bool bt_valid = false;									// We got valid data via bluetoth from the smartphone (GPS, time and date).
	float &lat = reinterpret_cast<float&>(mem[16]);			// GPS latidude.
	float &lon = reinterpret_cast<float&>(mem[20]);			// GPS longitude.
	uint16_t &spd = reinterpret_cast<uint16_t&>(mem[24]);	// GPS speed.
	int16_t &alt = reinterpret_cast<int16_t&>(mem[26]);		// GPS altitude.
	uint8_t &min = reinterpret_cast<uint8_t&>(mem[28]);		// Time and date.
	uint8_t &hour = reinterpret_cast<uint8_t&>(mem[29]);
	uint8_t &sec = reinterpret_cast<uint8_t&>(mem[30]);		// Sec and day are reverences to the same location. Day received via bluetooth gets overwritten with sec from RTC for storage on SD-Card.
	uint8_t &day = reinterpret_cast<uint8_t&>(mem[30]);
	uint8_t &month = reinterpret_cast<uint8_t&>(mem[31]);
	uint8_t &year = reinterpret_cast<uint8_t&>(mem[32]);
};

#endif /* MEMORY_H_ */
