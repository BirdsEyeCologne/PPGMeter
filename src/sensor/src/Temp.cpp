/*
 * Temp.cpp
 *
 *  Created on: Sep 7, 2019
 *      Author: BirdsEyeCologne@gmail.com
 */

#include <sensor/inc/Temp.h>

extern volatile uint32_t ms_cnt;

namespace sens{

// ************************************************************************
Temp::Temp() {

}

// ************************************************************************
Temp::~Temp() {}

// ************************************************************************
void Temp::setup(SENSOR nr) {

   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

   GPIO_InitTypeDef GPIO_InitStructure;

   //****************************************************
   if (nr == SENSOR::T1) {
      // software spi SCK.
      GPIO_InitStructure.GPIO_Pin = m_t1.SCK.Pin;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(m_t1.SCK.Port, &GPIO_InitStructure);

      // software spi CS.
      GPIO_InitStructure.GPIO_Pin = m_t1.CS.Pin;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(m_t1.CS.Port, &GPIO_InitStructure);

      // software spi SO.
      GPIO_InitStructure.GPIO_Pin = m_t1.SO.Pin;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(m_t1.SO.Port, &GPIO_InitStructure);

      /*
      // software spi VCC.
      GPIO_InitStructure.GPIO_Pin = m_t1.VCC.Pin;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(m_t1.VCC.Port, &GPIO_InitStructure);
*/

      // software spi GND.
      GPIO_InitStructure.GPIO_Pin = m_t1.GND.Pin;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(m_t1.GND.Port, &GPIO_InitStructure);

      // set initial states of the pins.
      //GPIO_ResetBits(m_t1.VCC.Port, m_t1.VCC.Pin);
      GPIO_ResetBits(m_t1.GND.Port, m_t1.GND.Pin);
      GPIO_ResetBits(m_t1.CS.Port, m_t1.CS.Pin);
      GPIO_ResetBits(m_t1.SCK.Port, m_t1.SCK.Pin);
   }

   //****************************************************
   if (nr == SENSOR::T2) {
      // software spi SCK.
      GPIO_InitStructure.GPIO_Pin = m_t2.SCK.Pin;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(m_t2.SCK.Port, &GPIO_InitStructure);

      // software spi CS.
      GPIO_InitStructure.GPIO_Pin = m_t2.CS.Pin;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(m_t2.CS.Port, &GPIO_InitStructure);

      // software spi SO.
      GPIO_InitStructure.GPIO_Pin = m_t2.SO.Pin;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
      GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(m_t2.SO.Port, &GPIO_InitStructure);

      /*
      // software spi VCC.
      GPIO_InitStructure.GPIO_Pin = m_t2.VCC.Pin;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(m_t2.VCC.Port, &GPIO_InitStructure);
*/

      // software spi GND.
      GPIO_InitStructure.GPIO_Pin = m_t2.GND.Pin;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
      GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_Init(m_t2.GND.Port, &GPIO_InitStructure);

      // set initial states of the pins.
      //GPIO_ResetBits(m_t2.VCC.Port, m_t2.VCC.Pin);
      GPIO_ResetBits(m_t2.GND.Port, m_t2.GND.Pin);
      GPIO_ResetBits(m_t2.CS.Port, m_t2.CS.Pin);
      GPIO_ResetBits(m_t2.SCK.Port, m_t2.SCK.Pin);
   }
}

// ************************************************************************
void Temp::power_off(SENSOR nr) {


   //****************************************************
   if (nr == SENSOR::T1) {
      GPIO_ResetBits(m_t1.GND.Port, m_t1.GND.Pin);
      GPIO_ResetBits(m_t1.CS.Port, m_t1.CS.Pin);
   }

   //****************************************************
   if (nr == SENSOR::T2) {
      GPIO_ResetBits(m_t2.GND.Port, m_t2.GND.Pin);
      GPIO_ResetBits(m_t2.CS.Port, m_t2.CS.Pin);
   }

}

// ************************************************************************
void Temp::power_on(SENSOR nr) {

   //****************************************************
   if (nr == SENSOR::T1) {
      GPIO_SetBits(m_t1.GND.Port, m_t1.GND.Pin);
      GPIO_SetBits(m_t1.CS.Port, m_t1.CS.Pin);
   }

   //****************************************************
   if (nr == SENSOR::T2) {
      GPIO_SetBits(m_t2.GND.Port, m_t2.GND.Pin);
      GPIO_SetBits(m_t2.CS.Port, m_t2.CS.Pin);
   }

   ms_cnt = 200;
   while (ms_cnt != 0);    // little delay for temp sensor to settle.

}

// ************************************************************************
float Temp::get_value(SENSOR nr) {

   uint16_t val = 0;

   //****************************************************
   if (nr == SENSOR::T1) {

      GPIO_ResetBits(m_t1.CS.Port, m_t1.CS.Pin);       // pull cs down to start a temp conversion.

      for (uint8_t delay = 0; delay < 40; delay++);   // little delay for temp conversion to finish before read.

      for (uint8_t cnt = 0; cnt < 16; cnt++) {        // read 16 bit of data from temp comversion.

         GPIO_SetBits(m_t1.SCK.Port, m_t1.SCK.Pin);          // clock hi.

         for (uint32_t delay = 0; delay < 10; delay++);

         val = val << 1;
         val |= GPIO_ReadInputDataBit(m_t1.SO.Port, m_t1.SO.Pin);   // read so pin.

         GPIO_ResetBits(m_t1.SCK.Port, m_t1.SCK.Pin);        // clock low.

         for (uint8_t delay = 0; delay < 15; delay++);
      }

      GPIO_SetBits(m_t1.CS.Port, m_t1.CS.Pin);               // cs hi, data from conversion was read.

      if(val & 0x04){                                        // check if sensor cable is plugged (D2 is high == unplugged)
         m_t1.cable_connected = false;
      }
   }

   //****************************************************
   if (nr == SENSOR::T2) {

      GPIO_ResetBits(m_t2.CS.Port, m_t2.CS.Pin);               // pull cs down to start a temp conversion.

      for (volatile uint8_t delay = 0; delay < 40; delay++);   // little delay for temp conversion to finish before read.

      for (volatile uint8_t cnt = 0; cnt < 16; cnt++) {        // read 16 bit of data from temp comversion.

         GPIO_SetBits(m_t2.SCK.Port, m_t2.SCK.Pin);            // clock hi.
         for (volatile uint32_t delay = 0; delay < 10; delay++);

         val = val << 1;
         val |= GPIO_ReadInputDataBit(m_t2.SO.Port, m_t2.SO.Pin);   // read so pin.

         GPIO_ResetBits(m_t2.SCK.Port, m_t2.SCK.Pin);          // clock low.
         for (volatile uint8_t delay = 0; delay < 15; delay++);
      }

      GPIO_SetBits(m_t2.CS.Port, m_t2.CS.Pin);                 // cs hi, data from conversion was read.

      if(val & 0x04){                                          // check if sensor cable is plugged (D2 is high == unplugged)
         m_t2.cable_connected = false;
      }
   }

   // value is in °C / 2^5
   return static_cast<float>(val) * 0.03125f;
}

// ************************************************************************
status_t Temp::bit(SENSOR nr) {

   status_t status = RC::NC;

   //****************************************************
   if (nr == SENSOR::T1) {

      uint16_t val = get_value(nr);

      if(val != 0 && m_t1.cable_connected == true){
         status = RC::OK;
      }
   }

   //****************************************************
   if (nr == SENSOR::T2) {

      uint16_t val = get_value(nr);

      if(val != 0 && m_t2.cable_connected == true){
         status = RC::OK;
      }
   }

   return status;
}

// ************************************************************
void Temp::spi2_setup(void) {

	/*
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;

    GPIO_DeInit(GPIOB);

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;              // CS
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;              // SCK
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;              // MISO
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;              // MOSI
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);

    SPI_I2S_DeInit(SPI2);
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_Init(SPI2, &SPI_InitStructure);
    SPI_Cmd(SPI2, ENABLE);
    */
}

}









