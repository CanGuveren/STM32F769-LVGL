/**
  ******************************************************************************
  * @file           : stm32f769_sdram.c
  * @brief          : SDRAM (FMC) driver for the STM32F769I-DISCO board.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f769_sdram.h"

/* Private define ------------------------------------------------------------*/
#define SDRAM_REFRESH_COUNT               1667U
#define SDRAM_MODE_REG_CL3_SINGLE_WRITE   0x0230U

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Runs the SDRAM power-up initialization sequence and programs the
  *         refresh rate. The FMC/SDRAM controller must already be configured
  *         with HAL_SDRAM_Init() before calling this function.
  * @param  hsdram: pointer to an initialized SDRAM handle
  * @retval None
  */
void SDRAM_InitSequence(SDRAM_HandleTypeDef *hsdram)
{
  FMC_SDRAM_CommandTypeDef cmd = {0};

  /* STEP 1 - CLK_ENABLE: the clock is now usable by the SDRAM + wait 1 ms */
  cmd.CommandMode            = FMC_SDRAM_CMD_CLK_ENABLE;
  cmd.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
  cmd.AutoRefreshNumber      = 1;
  cmd.ModeRegisterDefinition = 0;
  HAL_SDRAM_SendCommand(hsdram, &cmd, HAL_MAX_DELAY);
  HAL_Delay(1);

  /* STEP 2 - PALL: precharge all banks */
  cmd.CommandMode            = FMC_SDRAM_CMD_PALL;
  cmd.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
  cmd.AutoRefreshNumber      = 1;
  cmd.ModeRegisterDefinition = 0;
  HAL_SDRAM_SendCommand(hsdram, &cmd, HAL_MAX_DELAY);

  /* STEP 3 - AUTO REFRESH x8: start the internal refresh counter */
  cmd.CommandMode            = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
  cmd.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
  cmd.AutoRefreshNumber      = 8;
  cmd.ModeRegisterDefinition = 0;
  HAL_SDRAM_SendCommand(hsdram, &cmd, HAL_MAX_DELAY);

  /* STEP 4 - LOAD MODE: program the mode register (CAS latency = 3, burst settings) */
  cmd.CommandMode            = FMC_SDRAM_CMD_LOAD_MODE;
  cmd.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
  cmd.AutoRefreshNumber      = 1;
  cmd.ModeRegisterDefinition = SDRAM_MODE_REG_CL3_SINGLE_WRITE;
  HAL_SDRAM_SendCommand(hsdram, &cmd, HAL_MAX_DELAY);

  /* STEP 5 - REFRESH RATE: set the refresh counter of the FMC */
  HAL_SDRAM_ProgramRefreshRate(hsdram, SDRAM_REFRESH_COUNT);
}

/**
  * @brief  Checks that the whole SDRAM really stores and returns data, i.e.
  *         that the FMC configuration, the wiring and the chip itself work.
  * @note   This test is destructive: it overwrites the entire SDRAM contents.
  *         Call it before the framebuffer (or anything else living in SDRAM)
  *         is used.
  * @retval SDRAM_TEST_PASS if every written value was read back unchanged,
  *         SDRAM_TEST_FAIL at the first mismatch.
  */
SDRAM_TestStatus_t SDRAM_Test(void)
{
  volatile uint32_t *sdram     = (volatile uint32_t *)SDRAM_BASE;
  const uint32_t     wordCount = (uint32_t)(SDRAM_SIZE / sizeof(uint32_t));
  uint32_t           index;

  /* Phase 1 - Data bus test: write patterns that toggle every data line on a
     single word. A stuck-at or shorted DQ line shows up immediately. */
  static const uint32_t dataBusPattern[] =
  {
    0x00000000U, 0xFFFFFFFFU, 0x55555555U, 0xAAAAAAAAU
  };

  for (index = 0U; index < (sizeof(dataBusPattern) / sizeof(dataBusPattern[0])); index++)
  {
    sdram[0] = dataBusPattern[index];
    __DSB();
    if (sdram[0] != dataBusPattern[index])
    {
      return SDRAM_TEST_FAIL;
    }
  }

  /* Phase 2 - Address test: every word stores its own offset, so a broken or
     aliased address line produces a mismatch somewhere on read-back. */
  for (index = 0U; index < wordCount; index++)
  {
    sdram[index] = index;
  }
  __DSB();
  for (index = 0U; index < wordCount; index++)
  {
    if (sdram[index] != index)
    {
      return SDRAM_TEST_FAIL;
    }
  }

  /* Phase 3 - Inverted pattern: the same address test with all bits flipped,
     which also catches memory cells stuck at 0 or 1. */
  for (index = 0U; index < wordCount; index++)
  {
    sdram[index] = ~index;
  }
  __DSB();
  for (index = 0U; index < wordCount; index++)
  {
    if (sdram[index] != ~index)
    {
      return SDRAM_TEST_FAIL;
    }
  }

  return SDRAM_TEST_PASS;
}
