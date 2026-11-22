/**
  ******************************************************************************
  * @file    stm32f769_lcd.h
  * @brief   DSI/LTDC display driver and LVGL port for STM32F769I-DISCO.
  ******************************************************************************
  * Panel: OTM8009A, 800x480, DSI command mode.
  * Presentation: tear-free updates in 2 vertical bands, driven by TE.
  * Double buffering: FB0 = FB0_ADDRESS, FB1 = FB1_ADDRESS (FRAMEBUFFER0/1).
  ******************************************************************************
  */

#ifndef STM32F769_LCD_H
#define STM32F769_LCD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32f7xx_hal.h"

/* Panel Define Values */
#define LCD_PANEL_WIDTH      800U
#define LCD_PANEL_HEIGHT     480U
#define LCD_BYTES_PER_PIXEL  4U
#define LCD_ZONES            2U
#define LCD_ZONE_WIDTH       (LCD_PANEL_WIDTH / LCD_ZONES)



HAL_StatusTypeDef LCD_DSI_EnableAllCommands(DSI_HandleTypeDef *hdsi);  /* Transmit every DSI command type in low-power (LP) mode. */
HAL_StatusTypeDef LCD_DSI_DisableAllCommands(DSI_HandleTypeDef *hdsi); /* Transmit every DSI command type in high-speed (HS) mode (CubeMX default). */
void LCD_DisplayOff(DSI_HandleTypeDef *hdsi); /* Turn the panel off with a DCS DISPOFF command. */
void LCD_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* STM32F769_LCD_H */
