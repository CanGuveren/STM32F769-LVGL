/**
  ******************************************************************************
  * @file    stm32f769_lcd.c
  * @brief   DSI/LTDC display driver and LVGL port for STM32F769I-DISCO.
  ******************************************************************************
  * The panel runs in DSI command mode, so it holds its own GRAM and nothing is
  * scanned out automatically. Tear-free updates are built from the panel's TE
  * signal (see the block comment below).
  *
  * Double buffering is owned by LVGL: it decides which framebuffer to render
  * into and hands the finished one to this driver from the flush callback. The
  * driver presents it on the next TE and reports completion from the
  * end-of-refresh interrupt.
  ******************************************************************************
  */

#include "stm32f769_lcd.h"
#include "otm8009a.h"
#include "main.h"   /* FB0_ADDRESS, FB1_ADDRESS */
#include "lvgl.h"   /* the LVGL port lives in this file */

extern DSI_HandleTypeDef hdsi;
extern LTDC_HandleTypeDef hltdc;

static uint8_t pZoneCol[LCD_ZONES][4];					        /* CASET payload per band: XS[15:0] followed by XE[15:0]. */
static const uint8_t pZonePage[4] = {0x00, 0x00, 0x01, 0xDF};   /* PASET payload: full height 0..479, sent once in LCD_ZoneInit(). */
uint8_t pSyncLeft[2]   = {0x02, 0x15}; 							/* Scan @ 533 */
static volatile uint8_t LCD_ActiveZone = LCD_ZONES; 			/* Band being pushed; LCD_ZONES means no transfer in progress. */
static volatile uint8_t LCD_RefreshReq = 0U; 					/* A finished frame is waiting for the next TE. */
static volatile uint32_t LCD_FrontFB   = (uint32_t)FB0_ADDRESS;
static volatile uint32_t LCD_PendingFB = (uint32_t)FB0_ADDRESS;
static lv_display_t *s_disp = NULL;

static void LCD_DSI_BuildCmdConfig(DSI_LPCmdTypeDef *lpCmd, uint8_t lowPower)
{
  lpCmd->LPGenShortWriteNoP  = (lowPower != 0U) ? DSI_LP_GSW0P_ENABLE  : DSI_LP_GSW0P_DISABLE;
  lpCmd->LPGenShortWriteOneP = (lowPower != 0U) ? DSI_LP_GSW1P_ENABLE  : DSI_LP_GSW1P_DISABLE;
  lpCmd->LPGenShortWriteTwoP = (lowPower != 0U) ? DSI_LP_GSW2P_ENABLE  : DSI_LP_GSW2P_DISABLE;
  lpCmd->LPGenShortReadNoP   = (lowPower != 0U) ? DSI_LP_GSR0P_ENABLE  : DSI_LP_GSR0P_DISABLE;
  lpCmd->LPGenShortReadOneP  = (lowPower != 0U) ? DSI_LP_GSR1P_ENABLE  : DSI_LP_GSR1P_DISABLE;
  lpCmd->LPGenShortReadTwoP  = (lowPower != 0U) ? DSI_LP_GSR2P_ENABLE  : DSI_LP_GSR2P_DISABLE;
  lpCmd->LPGenLongWrite      = (lowPower != 0U) ? DSI_LP_GLW_ENABLE    : DSI_LP_GLW_DISABLE;
  lpCmd->LPDcsShortWriteNoP  = (lowPower != 0U) ? DSI_LP_DSW0P_ENABLE  : DSI_LP_DSW0P_DISABLE;
  lpCmd->LPDcsShortWriteOneP = (lowPower != 0U) ? DSI_LP_DSW1P_ENABLE  : DSI_LP_DSW1P_DISABLE;
  lpCmd->LPDcsShortReadNoP   = (lowPower != 0U) ? DSI_LP_DSR0P_ENABLE  : DSI_LP_DSR0P_DISABLE;
  lpCmd->LPDcsLongWrite      = (lowPower != 0U) ? DSI_LP_DLW_ENABLE    : DSI_LP_DLW_DISABLE;
  lpCmd->LPMaxReadPacket     = (lowPower != 0U) ? DSI_LP_MRDP_ENABLE   : DSI_LP_MRDP_DISABLE;
  lpCmd->AcknowledgeRequest  = (lowPower != 0U) ? DSI_ACKNOWLEDGE_ENABLE : DSI_ACKNOWLEDGE_DISABLE;
}

HAL_StatusTypeDef LCD_DSI_EnableAllCommands(DSI_HandleTypeDef *hdsi)
{
  DSI_LPCmdTypeDef lpCmd = {0};

  LCD_DSI_BuildCmdConfig(&lpCmd, 1U);

  return HAL_DSI_ConfigCommand(hdsi, &lpCmd);
}


HAL_StatusTypeDef LCD_DSI_DisableAllCommands(DSI_HandleTypeDef *hdsi)
{
  DSI_LPCmdTypeDef lpCmd = {0};

  LCD_DSI_BuildCmdConfig(&lpCmd, 0U);

  return HAL_DSI_ConfigCommand(hdsi, &lpCmd);
}

void LCD_DisplayOff(DSI_HandleTypeDef *hdsi)
{
  HAL_DSI_ShortWrite(hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, OTM8009A_CMD_DISPOFF, 0x00); //Turn the panel off with a DCS DISPOFF command.
}

static void LCD_BuildZoneCols(void) // Build the CASET payload of every band.
{
  uint8_t i;

  for (i = 0U; i < LCD_ZONES; i++)
  {
    uint16_t xs = (uint16_t)((uint16_t)i * LCD_ZONE_WIDTH);
    uint16_t xe = (uint16_t)(xs + LCD_ZONE_WIDTH - 1U);

    pZoneCol[i][0] = (uint8_t)(xs >> 8);
    pZoneCol[i][1] = (uint8_t)(xs & 0xFFU);
    pZoneCol[i][2] = (uint8_t)(xe >> 8);
    pZoneCol[i][3] = (uint8_t)(xe & 0xFFU);
  }
}

static void LCD_SetUpdateRegion(uint8_t zone)
{
  HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 4U, OTM8009A_CMD_CASET, pZoneCol[zone]);
}

static void LCD_ZoneInit(void)
{
  LCD_BuildZoneCols();

  HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, sizeof(pZonePage), OTM8009A_CMD_PASET, (uint8_t *)pZonePage);
  HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 2U, OTM8009A_CMD_WRTESCN, pSyncLeft);
  HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, OTM8009A_CMD_TEEON, OTM8009A_TEEON_TELOM_VBLANKING_INFO_ONLY);

  LCD_FrontFB   = (uint32_t)FB0_ADDRESS;
  LCD_PendingFB = LCD_FrontFB;
  LTDC_LAYER(&hltdc, 0)->CFBAR = LCD_FrontFB;
  __HAL_LTDC_RELOAD_IMMEDIATE_CONFIG(&hltdc);
  LCD_SetUpdateRegion(0U);

  LCD_ActiveZone = LCD_ZONES;
  LCD_RefreshReq = 0U;
}

static void LCD_RequestRefresh(const void *framebuffer)
{

  LCD_PendingFB  = (uint32_t)framebuffer;
  LCD_RefreshReq = 1U;
}


static void LCD_Flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
  LV_UNUSED(area);

  if (lv_display_flush_is_last(disp))
  {
    __DSB();
    LCD_RequestRefresh(px_map);
  }
  else
  {
    lv_display_flush_ready(disp);
  }
}

void LCD_Init(void)
{
  LCD_ZoneInit();

  lv_init();
  lv_tick_set_cb(HAL_GetTick);

  s_disp = lv_display_create(LCD_PANEL_WIDTH, LCD_PANEL_HEIGHT);
  lv_display_set_color_format(s_disp, LV_COLOR_FORMAT_ARGB8888); //Color format must be same LTDC config.
  lv_display_set_flush_cb(s_disp, LCD_Flush);
  lv_display_set_buffers(s_disp, (void *)FB0_ADDRESS, (void *)FB1_ADDRESS, (uint32_t)LCD_BYTES_PER_PIXEL * LCD_PANEL_WIDTH * LCD_PANEL_HEIGHT, LV_DISPLAY_RENDER_MODE_DIRECT);
}

void HAL_DSI_TearingEffectCallback(DSI_HandleTypeDef *hdsi)
{
  if (LCD_RefreshReq != 0U)
  {
    LCD_FrontFB = LCD_PendingFB;

    __HAL_DSI_WRAPPER_DISABLE(hdsi);
    LTDC_LAYER(&hltdc, 0)->CFBAR = LCD_FrontFB;
    __HAL_LTDC_RELOAD_IMMEDIATE_CONFIG(&hltdc);
    __HAL_DSI_WRAPPER_ENABLE(hdsi);

    LCD_ActiveZone = 1U;
    HAL_DSI_Refresh(hdsi);
    LCD_RefreshReq = 0U;
  }
}

void HAL_DSI_EndOfRefreshCallback(DSI_HandleTypeDef *hdsi)
{
  if (LCD_ActiveZone < LCD_ZONES)
  {
    __HAL_DSI_WRAPPER_DISABLE(hdsi);
    LTDC_LAYER(&hltdc, 0)->CFBAR = LCD_FrontFB + ((uint32_t)LCD_ActiveZone * LCD_ZONE_WIDTH * LCD_BYTES_PER_PIXEL);
    __HAL_LTDC_RELOAD_IMMEDIATE_CONFIG(&hltdc);
    __HAL_DSI_WRAPPER_ENABLE(hdsi);

    LCD_SetUpdateRegion(LCD_ActiveZone);
    LCD_ActiveZone++;
    HAL_DSI_Refresh(hdsi);
  }
  else
  {
    __HAL_DSI_WRAPPER_DISABLE(hdsi);
    LTDC_LAYER(&hltdc, 0)->CFBAR = LCD_FrontFB;
    __HAL_LTDC_RELOAD_IMMEDIATE_CONFIG(&hltdc);
    __HAL_DSI_WRAPPER_ENABLE(hdsi);

    LCD_SetUpdateRegion(0U);

    if (s_disp != NULL)
    {
      lv_display_flush_ready(s_disp);
    }
  }
}
