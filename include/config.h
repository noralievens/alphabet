/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @file        : config.h
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <gtk/gtk.h>

#define UNUSED __attribute__((unused))

#define ICON_SIZE   GTK_ICON_SIZE_MENU
#define MARGIN      12

#define WINDOW_X    720
#define WINDOW_Y    400

#define COLOR_TIMELINE_POSITION     "rgba(128,128,128,0.5)"
#define COLOR_TIMELINE_LOOP         "rgba(0,128,0,0.25)"
#define COLOR_TIMELINE_MARKER       "rgba(128,0,0,0.5)"

extern void dtoduration(char* dest, double num);

#endif

// vim:ft=c

