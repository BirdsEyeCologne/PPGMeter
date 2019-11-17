/*
 * DataStorage.cpp
 *
 *  Created on: Sep 7, 2019
 *      Author: BirdsEyeCologne@gmail.com
 */

#include <system/inc/DataStorage.h>

namespace sys {

// ************************************************************************
DataStorage::DataStorage() {
}

// ************************************************************************
DataStorage::~DataStorage() {
	umount();
}

// ************************************************************************
status_t DataStorage::open(RTC_DateTypeDef today) {

	status_t status = RC::ERROR;
	FRESULT res = FR_OK;
	FILINFO fno;

	// Create year folder if nessesary.
	if (res == FR_OK) {

		sprintf(m_path, "%d", (uint16_t) today.RTC_Year + 2000);

		res = f_stat(m_path, &fno);

		if (res != FR_OK) {
			res = f_mkdir(m_path);
		}

		res = f_chdir(m_path);
	}

	// Create month folder if nessesary.
	if (res == FR_OK) {

		sprintf(m_path, "%d", today.RTC_Month);

		res = f_stat(m_path, &fno);

		if (res != FR_OK) {
			res = f_mkdir(m_path);
		}

		res = f_chdir(m_path);
	}

	// Create day of month folder if nessesary.
	if (res == FR_OK) {

		sprintf(m_path, "%d", today.RTC_Date);

		res = f_stat(m_path, &fno);

		if (res != FR_OK) {
			res = f_mkdir(m_path);
		}

		res = f_chdir(m_path);
	}

	// Check if there are other flights in the folder and create a new fligth file.
	if (res == FR_OK) {

		for (uint16_t fnumber = 1; fnumber <= FILES_PER_DAY_MAX; fnumber++) {

			sprintf(m_path, "%d.bin", fnumber);

			res = f_stat(m_path, &fno);

			// Create file.
			if (res == FR_NO_FILE) {
				res = f_open(&m_file, m_path, FA_CREATE_ALWAYS | FA_WRITE);
				break;
			}
		}
	}

	if (res == FR_OK) {
		status = RC::OK;
	}

	return status;
}

// ************************************************************************
status_t DataStorage::write(uint8_t *data, uint16_t size) {

	status_t status = RC::ERROR;

	FRESULT res;

	UINT written = 0;

	res = f_write(&m_file, data, size, &written);

	if (res == FR_OK && size == written) {
		status = RC::OK;
	}

	return status;
}

// ************************************************************************
status_t DataStorage::close() {

	status_t status = RC::ERROR;

	FRESULT res;

	res = f_close(&m_file);

	res = f_chdir(logging_folder);

	if (res == FR_OK) {
		status = RC::OK;
	}

	return status;
}

// ************************************************************************
void DataStorage::setup(void) {
	UB_SDCard_Init();
}

// ************************************************************************
FRESULT DataStorage::mount(FATFS *fs) {

	FRESULT status = FR_DISK_ERR;

	status = f_mount(fs, "", 1);

	return status;
}

// ************************************************************************
status_t DataStorage::bit() {

	status_t status = RC::OK;
	FRESULT res;

	res = mount(&m_fs);

	if (res == FR_OK) {
		res = create_folders();
	}

	if (res != FR_OK) {
		status = RC::FAIL;
	}

	return status;
}

// ************************************************************************
FRESULT DataStorage::umount() {

	FRESULT status = FR_DISK_ERR;

	status = f_mount(0, "", 1);

	return status;
}

// ************************************************************************
FRESULT DataStorage::create_folders() {

	FRESULT res;
	FILINFO fno;

	res = f_stat(root_folder, &fno);

	if (res != FR_OK) {
		res = f_mkdir(root_folder);
	}

	res = f_chdir(root_folder);

	res = f_stat(fw_folder, &fno);

	if (res != FR_OK) {
		res = f_mkdir(fw_folder);
	}

	res = f_stat(data_folder, &fno);

	if (res != FR_OK) {
		res = f_mkdir(data_folder);
	}

	res = f_chdir(data_folder);

	return res;
}

}

