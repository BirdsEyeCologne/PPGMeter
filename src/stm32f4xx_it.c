/**
 ******************************************************************************
 * @file    Project/STM32F4xx_StdPeriph_Template/stm32f4xx_it.c
 * @author  MCD Application Team
 * @version V1.1.0
 * @date    18-January-2013
 * @brief   Main Interrupt Service Routines.
 *          This file provides template for all exceptions handler and
 *          peripherals interrupt service routine.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "Global.h"
/** @addtogroup Template_Project
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint16_t tmpCC4[2] = { 0, 0 };
static uint32_t em_halt_cnt = 0;
static uint16_t rpm_debounced = 0;
static uint8_t rpm_is_low = 0;
static uint8_t rpm_is_high = 0;
static uint32_t us100_clock = 0;

/* External variables ---------------------------------------------------------*/
extern __IO uint32_t ms_cnt;
extern __IO uint32_t check_rpm;
extern __IO uint32_t ms_clock;
extern __IO uint32_t rpm_cnt;
extern __IO uint32_t uwPeriodValue;
extern __IO uint32_t uwCaptureNumber;
extern __IO uint8_t uart1_rx_buff[];
extern __IO uint8_t uart1_rx_cnt;
extern __IO uint8_t time_sync;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief   This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void) {
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void) {
	/* Go to infinite loop when Hard Fault exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void) {
	/* Go to infinite loop when Memory Manage exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void) {
	/* Go to infinite loop when Bus Fault exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void) {
	/* Go to infinite loop when Usage Fault exception occurs */
	while (1) {
	}
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler(void) {
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void) {
}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler(void) {
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void) {
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f40xx.s/startup_stm32f427x.s).                         */
/******************************************************************************/

/**
 * @brief  Meas for WD - This function handles TIM5 global interrupt request.
 * @param  None
 * @retval None
 */
// ************************************************************
void TIM5_IRQHandler(void) {
	if (TIM_GetITStatus(TIM5, TIM_IT_CC4) != RESET) {
		/* Get the Input Capture value */
		tmpCC4[uwCaptureNumber++] = TIM_GetCapture4(TIM5);

		/* Clear CC4 Interrupt pending bit */
		TIM_ClearITPendingBit(TIM5, TIM_IT_CC4);

		if (uwCaptureNumber >= 2) {
			/* Compute the period length */
			uwPeriodValue = (uint16_t) (0xFFFF - tmpCC4[0] + tmpCC4[1] + 1);
		}
	}
}

/**
 * @brief  1 ms counter - This function handles TIM4 global interrupt request.
 * @param  None
 * @retval None
 */
// ************************************************************
void TIM4_IRQHandler(void) {

	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

	if(ms_cnt){
		ms_cnt--;
	}

	//ms_cnt -= (ms_cnt != 0) ? 1 : 0;	// same same...

	ms_clock++;

	#ifdef DEBUG_RPM_ENABLE
	if (ms_clock % 4 == 0) {
		GPIO_ToggleBits(GPIOD, GPIO_Pin_15);
	}
	#endif
}

/**
 * @brief  100 us counter - This function handles TIM2 global interrupt request.
 * @param  None
 * @retval None
 */
// ************************************************************
void TIM2_IRQHandler(void) {

	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

	us100_clock++;

	// Shift old pin states.
	rpm_debounced <<= 1;

	// Save new pin state of RPM pin.
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == Bit_SET) {
		rpm_debounced |= 1;
	}

	// Debounced low detected.
	if ( (rpm_debounced & BIT_MASK) == 0x0 && rpm_is_low == 0) {
		rpm_is_low = 1;
	}

	// Debounced rising edge, after low detected.
	if ( (rpm_debounced & BIT_MASK) == BIT_MASK && rpm_is_high == 0 && rpm_is_low == 1) {
		rpm_is_high = 1;
	}

	// Debounced edge detected.
	if (rpm_is_low == 1 && rpm_is_high == 1) {
		rpm_is_low = 0;
		rpm_is_high = 0;
		rpm_cnt++;

		// Check for to high RPMs, by measruing the time between two ignitions.
		#ifdef SAVE_START
		if (check_rpm == TRUE) {
			// Time delta between two iginitions is to small => RPM is to high!
			if (us100_clock < RPM_EM_HALT_US) {
				em_halt_cnt++;
				// To many high RPM measured => Halt motor.
				if (em_halt_cnt >= RPM_EM_HALT_CNT) {
					// Short the ignition coil, like the kill switch does.
					GPIO_SetBits(GPIOA, GPIO_Pin_0);   // Emergency Halt !!
					check_rpm = FALSE;
				}
			}
		}
		us100_clock = 0;
		#endif

	}

}

/**
 * @brief  BT-Com - This function handles USART1 global interrupt request.
 * @param  None
 * @retval None
 */
// ************************************************************
void USART1_IRQHandler(void) {
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
		if (uart1_rx_cnt < UART1_BUFFER_SIZE) {
			uart1_rx_buff[uart1_rx_cnt++] = USART_ReceiveData(USART1);
			//USART1->DR = rx_buff[rx_cnt];	// echo the received data
		}
		//uart1_rx_cnt = (uart1_rx_cnt < UART1_BUFFER_SIZE) ? (uart1_rx_cnt + 1) : 0;
	}
}

/**
 * @brief  K1 - This function handles External line 3 interrupt request.
 * @param  None
 * @retval None
 */
// ************************************************************
void EXTI3_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line3) == SET) {

		EXTI_ClearITPendingBit(EXTI_Line3);

		GPIO_SetBits(GPIOA, GPIO_Pin_1);   // Emergency Halt pin!
	}
}

/**
 * @brief  K0 - This function handles External line 4 interrupt request.
 * @param  None
 * @retval None
 */
// ************************************************************
void EXTI4_IRQHandler(void) {
	if (EXTI_GetITStatus(EXTI_Line4) == SET) {

		EXTI_ClearITPendingBit(EXTI_Line4);

		time_sync = 1;

		//PWR_EnterSTANDBYMode();
		// generate seg fault
		//*(__IO uint32_t *) 0xA0001000 = 0xFF;
	}
}

/**
 * @brief  RPM/K_UP - This function handles External line 0 interrupt request.
 * If btn K_UP is pressed in Standby, the controler wakes up. Also a RPM
 * rising edge might trigger the IRQHandler.
 * @param  None
 * @retval None
 */
/*
 // ************************************************************
 void EXTI0_IRQHandler(void) {

 EXTI_ClearITPendingBit(EXTI_Line0);

 }
 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

