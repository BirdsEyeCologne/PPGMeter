/*
 * BtCom.h
 *
 *  Created on: Sep 7, 2019
 *      Author: fox
 */

#ifndef BTCOM_H_
#define BTCOM_H_

#include <cstdint>
#include <cstring>

#include "stm32f4xx.h"
#include "../../Global.h"
#include "../../ReturnCodes.h"

#define BT_PAYLOAD 18

enum class RESP_HDR : uint8_t {REPORT_DATA = 0xAD};
typedef RESP_HDR resp_hdr_t;

enum class HDR : uint8_t {NO_CMD = 0x00, LOC_TIM = 0xAA};
typedef HDR hdr_t;

enum class BAUD_RATE : uint32_t {COM = 9600, AT = 38400};

namespace sys{

// ************************************************************************
class BtCom {
public:
	BtCom();
	virtual ~BtCom();
	void power_on();
	void power_off();
	void setup(BAUD_RATE br);
	void tx_header(resp_hdr_t header);
	void tx_data(uint8_t * arr, uint8_t arr_len);
	hdr_t rx_data(uint8_t * data);
	status_t bit(void);
    void set_at_mode(bool state);
private:
	uint8_t m_at_reset [11];
};

}
#endif /* BTCOM_H_ */
