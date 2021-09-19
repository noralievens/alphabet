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



extern void dtoduration(char* dest, double num);

#endif

// vim:ft=c

