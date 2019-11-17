/*
 * Pressure.h
 *
 *  Created on: Sep 25, 2019
 *      Author: fox
 */

#ifndef PRESSURE_H_
#define PRESSURE_H_

#include <cstdint>
#include "stm32f4xx.h"
#include "../../ReturnCodes.h"
#include "../../../Libraries/BMP280/bmp280_defs.h"
#include "../../../Libraries/BMP280/bmp280.h"

namespace sens{

// ************************************************************************
class Pressure {
public:
   Pressure();
   virtual ~Pressure();
   void setup(void);
   void power_off();
   void power_on();
   status_t bit();
   int8_t get_values(uint32_t & pres, int16_t & temp);
private:
   static int8_t spi_reg_write(uint8_t cs, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
   static int8_t spi_reg_read(uint8_t cs, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
   static void delay_ms(uint32_t period_ms);
private:
   struct bmp280_dev m_bmp;
   struct bmp280_config m_conf;
};

}
#endif /* PRESSURESENS_H_ */
