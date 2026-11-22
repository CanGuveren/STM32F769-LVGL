/**
 * @file lvgl_cluster_ui.h
 */

#ifndef LVGL_PRO_LVGL_CLUSTER_UI_H
#define LVGL_PRO_LVGL_CLUSTER_UI_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      DEFINES
 *********************/

/* Target(s) select at startup: one or more LVGL_CLUSTER_UI_TARGET_* values OR-ed together. */
#ifndef LVGL_CLUSTER_UI_COMPILE_TARGET
#define LVGL_CLUSTER_UI_COMPILE_TARGET LVGL_CLUSTER_UI_TARGET_ALL
#endif

/*********************
 *      INCLUDES
 *********************/

#include "lvgl_cluster_ui_gen.h"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL VARIABLES
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize the component library
 */
void lvgl_cluster_ui_init(const char * asset_path);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PRO_LVGL_CLUSTER_UI_H*/