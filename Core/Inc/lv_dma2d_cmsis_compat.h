/**
 * @file lv_dma2d_cmsis_compat.h
 * @brief CMSIS compatibility layer for LVGL's DMA2D draw unit.
 */

#ifndef LV_DMA2D_CMSIS_COMPAT_H
#define LV_DMA2D_CMSIS_COMPAT_H

#include "stm32f7xx_hal.h"

/*
 * The CMSIS core header of this firmware package declares the cache maintenance
 * helpers with a `uint32_t *` address, while LVGL 9.6 passes a `uint8_t *`.
 * GCC 14 rejects that implicit conversion. The generated instructions are the
 * same, so the address is cast at the call sites instead of patching the LVGL
 * submodule. Only the LVGL DMA2D translation units see these macros, because
 * this header is referenced solely through LV_DRAW_DMA2D_HAL_INCLUDE.
 */
#define SCB_CleanDCache_by_Addr(addr, dsize)      SCB_CleanDCache_by_Addr((uint32_t *)(addr), (dsize))
#define SCB_InvalidateDCache_by_Addr(addr, dsize) SCB_InvalidateDCache_by_Addr((uint32_t *)(addr), (dsize))

#endif /* LV_DMA2D_CMSIS_COMPAT_H */
