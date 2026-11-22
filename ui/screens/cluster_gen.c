/**
 * @file cluster_gen.c
 * @brief Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/

#include "cluster_gen.h"
#include "../lvgl_cluster_ui.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

static void cluster_needle_rotation_binding_cb(lv_observer_t * observer, lv_subject_t * subject);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * cluster_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * the_root = NULL;

    #if LVGL_CLUSTER_UI_CHECK_COMPILE_TARGET(LVGL_CLUSTER_UI_TARGET_ALL)
    if (lvgl_cluster_ui_check_target(LVGL_CLUSTER_UI_TARGET_ALL)) {
        lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
        lv_obj_set_name_static(lv_obj_0, "cluster_#");

        lv_obj_t * lv_image_0 = lv_image_create(lv_obj_0);
        lv_image_set_src(lv_image_0, power_gauge);
        lv_obj_set_align(lv_image_0, LV_ALIGN_TOP_LEFT);
        lv_obj_set_x(lv_image_0, 328);
        lv_obj_set_y(lv_image_0, 4);

        lv_obj_t * needle = lv_image_create(lv_obj_0);
        lv_obj_set_name(needle, "needle");
        lv_image_set_src(needle, needle_small);
        lv_obj_set_align(needle, LV_ALIGN_TOP_LEFT);
        lv_obj_set_x(needle, 559);
        lv_obj_set_y(needle, 240);
        lv_image_set_pivot_x(needle, lv_pct(50));
        lv_image_set_pivot_y(needle, 0);
        lv_subject_add_observer_obj(subject_needle_angle, cluster_needle_rotation_binding_cb, needle, NULL);
        lv_image_set_antialias(needle, true);

        lv_obj_t * lv_image_1 = lv_image_create(lv_obj_0);
        lv_image_set_src(lv_image_1, inner_group);
        lv_obj_set_align(lv_image_1, LV_ALIGN_TOP_LEFT);
        lv_obj_set_x(lv_image_1, 460);
        lv_obj_set_y(lv_image_1, 136);

        the_root = lv_obj_0;
    }
    #endif

    LV_TRACE_OBJ_CREATE("finished");

    return the_root;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void cluster_needle_rotation_binding_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * obj = lv_observer_get_target_obj(observer);
    lv_image_set_rotation(obj, lv_subject_get_int(subject_needle_angle) * 10);
}

