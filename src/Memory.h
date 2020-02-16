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

	static constexpr uint32_t SIZE = 17;	// Total amount of Memory used for the received data.
	std::array<uint8_t, SIZE> recv = {0};	// Memory for received data relevant for the system.

	// Sensor data gathered at uc
	float tempCht;         // #1 MAX6675 temperature value of CHT in °C.
	float tempEgt;         // #2 MAX6675 temperature value of EGT in °C.
	float tempAir;         // BMP280 temperature value of air in °C.
	float pressureAir;     // BMP280 air pressure value in hPa.
	uint16_t rpmMotor;     // Rounds Per Minute (RPM) spinning of the motor.
	uint32_t runtimeMotor; // Cumulated total time the motor is/has been spinning in seconds.

	// Sensor data gathered at smartphone and send to uc via bluetooth.
	bool bt_valid = false;									// We got valid data via bluetoth from the smartphone (GPS, time and date).
	float &lat = reinterpret_cast<float&>(recv[0]);			// GPS latidude.
	float &lon = reinterpret_cast<float&>(recv[4]);			// GPS longitude.
	uint16_t &spd = reinterpret_cast<uint16_t&>(recv[8]);	// GPS speed.
	int16_t &alt = reinterpret_cast<int16_t&>(recv[10]);		// GPS altitude.
	uint8_t &min = reinterpret_cast<uint8_t&>(recv[12]);		// Time and date.
	uint8_t &hour = reinterpret_cast<uint8_t&>(recv[13]);
	uint8_t &sec = reinterpret_cast<uint8_t&>(recv[14]);		// Sec and day are reverences to the same location. Day received via bluetooth gets overwritten with sec from RTC for storage on SD-Card.
	uint8_t &day = reinterpret_cast<uint8_t&>(recv[14]);
	uint8_t &month = reinterpret_cast<uint8_t&>(recv[15]);
	uint8_t &year = reinterpret_cast<uint8_t&>(recv[16]);
};

#endif /* MEMORY_H_ */
