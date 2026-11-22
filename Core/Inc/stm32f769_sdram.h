/**
  ******************************************************************************
  * @file           : stm32f769_sdram.h
  * @brief          : SDRAM (FMC) driver for the STM32F769I-DISCO board.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F769_SDRAM_H
#define __STM32F769_SDRAM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/* Exported constants --------------------------------------------------------*/
/* FMC SDRAM bank 1 memory-mapped region */
#define SDRAM_BASE      0xC0000000UL
#define SDRAM_SIZE      (16UL * 1024UL * 1024UL)

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  Result of SDRAM_Test().
  */
typedef enum
{
  SDRAM_TEST_FAIL = 0,  /*!< At least one read returned a wrong value: the SDRAM is faulty. */
  SDRAM_TEST_PASS = 1   /*!< Every location returned the value that was written: the SDRAM is OK. */
} SDRAM_TestStatus_t;

/* Exported functions prototypes ---------------------------------------------*/
void SDRAM_InitSequence(SDRAM_HandleTypeDef *hsdram);
SDRAM_TestStatus_t SDRAM_Test(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F769_SDRAM_H */
