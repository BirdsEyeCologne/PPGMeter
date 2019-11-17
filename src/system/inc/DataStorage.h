/*
 * DataStorage.h
 *
 *  Created on: Sep 7, 2019
 *      Author: fox
 */

#ifndef DATASTORAGE_H_
#define DATASTORAGE_H_

#include <cstdio>
#include <cstdint>

#include "../Libraries/FatFs/driver/stm32_sdio.h"
#include "../Libraries/FatFs/ff.h"
#include "../../ReturnCodes.h"
#include "../../Global.h"

namespace sys{

constexpr char root_folder [] = "PPGMeter";
constexpr char fw_folder [] = "firmware";
constexpr char data_folder [] = "flights";
constexpr char logging_folder [] = "/PPGMeter/flights";


// ************************************************************************
class DataStorage {

public:
	DataStorage();
	virtual ~DataStorage();
	void setup(void);
	status_t bit();

	status_t open(RTC_DateTypeDef today);
	status_t write(uint8_t *data, uint16_t size);
	status_t close();
	FRESULT umount(void);

private:
    FRESULT mount(FATFS *fs);
    FRESULT create_folders();
private:
	FATFS m_fs;    // the file system.
	FIL m_file;    // only file open at a time, so this on is the only file pointer.
	char m_path[PATH_MAX];
};

}
#endif /* DATASTORAGE_H_ */
