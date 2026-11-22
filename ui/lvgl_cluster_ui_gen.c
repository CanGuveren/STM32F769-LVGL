/**
 * @file lvgl_cluster_ui_gen.c
 */

/*********************
 *      INCLUDES
 *********************/

#include "lvgl_cluster_ui.h"

#if defined(LV_USE_XML) && LV_USE_XML
#endif /* LV_USE_XML */

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void check_font(lv_font_t ** font, const char * name);

/**********************
 *  STATIC VARIABLES
 **********************/

static uint32_t lvgl_cluster_ui_target = LVGL_CLUSTER_UI_COMPILE_TARGET;

/*----------------
 * Translations
 *----------------*/

#ifndef LV_EDITOR_PREVIEW
    static const char * translation_languages[] = {"en", "de", NULL};
    static const char * translation_tags[] = {"dog", "cat", "house", NULL};
    static const char * translation_texts[] = {
        "This is a dog", "Das ist ein Hund", /* dog */
        "A curious little cat", "Eine neugierige kleine Katze", /* cat */
        "The house is cozy and warm", "Das Haus ist gemütlich und warm", /* house */
    };
#endif

/**********************
 *  GLOBAL VARIABLES
 **********************/

/*--------------------
 *  Permanent screens
 *-------------------*/

/*----------------
 * Fonts
 *----------------*/



/*----------------
 * Images
 *----------------*/

/* Targets: any */
const void * power_gauge = NULL;
extern const void * power_gauge_data;
const void * needle_small = NULL;
extern const void * needle_small_data;
const void * inner_group = NULL;
extern const void * inner_group_data;

/*----------------
 * Global styles
 *----------------*/

/*----------------
 * Subjects
 *----------------*/

lv_subject_t * subject_needle_angle;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lvgl_cluster_ui_init_gen(const char * asset_path)
{
    /* When running from the editor the theme set from the XML should overwrite this */
#if !defined(LV_EDITOR_PREVIEW)
#if LV_USE_THEME_SIMPLE
    lv_display_t * disp = lv_display_get_default();
    lv_theme_t * th = lv_theme_simple_init(disp);
    lv_display_set_theme(disp, th);
#else
    LV_LOG_WARN("Simple theme is selected in project.xml but LV_USE_THEME_SIMPLE is disabled");
#endif
#endif /*LV_EDITOR_PREVIEW*/


    /*----------------
     * Fonts
     *----------------*/




    /*----------------
     * Images
     *----------------*/

    /* Targets: any */
    #if LVGL_CLUSTER_UI_CHECK_COMPILE_TARGET(LVGL_CLUSTER_UI_TARGET_ALL)
    if (lvgl_cluster_ui_check_target(LVGL_CLUSTER_UI_TARGET_ALL)) {
        /* power_gauge */
        if (!power_gauge) {
            power_gauge = &power_gauge_data;
        }
        /* needle_small */
        if (!needle_small) {
            needle_small = &needle_small_data;
        }
        /* inner_group */
        if (!inner_group) {
            inner_group = &inner_group_data;
        }
    }
    #endif

    /*----------------
     * Global styles
     *----------------*/

    /*----------------
     * Subjects
     *----------------*/
    subject_needle_angle = lv_subject_create(LV_SUBJECT_TYPE_INT);
    lv_subject_set_min_value_int(subject_needle_angle, 0);
    lv_subject_set_max_value_int(subject_needle_angle, 360);
    lv_subject_set_int(subject_needle_angle, 0);

    /*----------------
     * Translations
     *----------------*/

    #ifndef LV_EDITOR_PREVIEW
        lv_translation_add_static(translation_languages, translation_tags, translation_texts);
        lv_translation_set_language(translation_languages[0]);
    #endif

#if defined(LV_USE_XML) && LV_USE_XML
    /* Register widgets */


    /* Register fonts */

    /* Register subjects */
    lv_xml_register_subject(NULL, "subject_needle_angle", subject_needle_angle);

    /* Register callbacks */

    /* Register easing callbacks */
#endif

    /* Register all the global assets so that they won't be created again when globals.xml is parsed.
     * While running in the editor skip this step to update the preview when the XML changes */
#if defined(LV_USE_XML) && LV_USE_XML && !defined(LV_EDITOR_PREVIEW)
    /* Register images */
    lv_xml_register_image(NULL, "power_gauge", power_gauge);
    lv_xml_register_image(NULL, "needle_small", needle_small);
    lv_xml_register_image(NULL, "inner_group", inner_group);
#endif

#if !defined(LV_USE_XML) || LV_USE_XML == 0
    /*--------------------
     *  Permanent screens
     *-------------------*/
    /* If XML is enabled it's assumed that the permanent screens are created
     * manually from XML using lv_xml_create() */
#endif
}

void lvgl_cluster_ui_set_target(uint32_t target)
{
    lvgl_cluster_ui_target = target;
}

uint32_t lvgl_cluster_ui_get_target(void)
{
    return lvgl_cluster_ui_target;
}

bool lvgl_cluster_ui_check_target(uint32_t target)
{
    return (lvgl_cluster_ui_target & target) ? true : false;
}

/* Callbacks */

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void check_font(lv_font_t ** font, const char * name)
{
    if (!(*font)) {
        *font = (lv_font_t *)LV_FONT_DEFAULT;
        LV_LOG_WARN("font `%s` was not set. Using `LV_FONT_DEFAULT` instead", name);
    }
}