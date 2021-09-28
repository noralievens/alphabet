/**
 * @author      Arno Lievens (arnolievens@gmail.com)
 * @date        08/09/2021
 * @file        config.h
 * @brief       configuration variables
 * @copyright   Copyright (c) 2021 Arno Lievens
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <gtk/gtk.h>

#define UNUSED __attribute__((unused))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define CLAMP(x, low, high) (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#define ELEMENTS(arr) (sizeof (arr) / sizeof ((arr)[0]))

/**
 * icons size for buttons
 */
#define ICON_SIZE                   GTK_ICON_SIZE_MENU

/**
 * margin used to seperate widgets (mainly buttons)
 */
#define MARGIN                      12

/**
 * default window width
 */
#define WINDOW_X                    720

/**
 * default window height
 */
#define WINDOW_Y                    400

/**
 * color used to draw timeline position cursor
 */
#define COLOR_TIMELINE_POSITION     "rgba(128,128,128,0.5)"

/**
 * color used to draw timeline loop cursor and area
 */
#define COLOR_TIMELINE_LOOP         "rgba(0,128,0,0.25)"

/**
 * color used to draw timeline marker
 */
#define COLOR_TIMELINE_MARKER       "rgba(128,0,0,0.5)"

/**
 * color used to draw timeline waveform
 */
#define COLOR_TIMELINE_WAVE         "rgba(206,106,29,0.5)"

/**
 * waveform in timeline is offset so that the avg LUFs is
 * at TIMELINE_AVG_HEIGHT * height of the widget (ref from the top)
 *
 * eg 0.5 will shift the waveform so that the avg lufs is always halfway.
 * lower value means waveforms will be highter up
 */
#define TIMELINE_AVG_HEIGHT         0.5

/**
 * Convert double to duration string
 *
 * output format is mm:ss.xxx
 *
 * @param dest string destination
 * @param num double to convert
 */
extern void dtoduration(char* dest, double num);

#endif
