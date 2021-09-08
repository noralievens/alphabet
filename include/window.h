/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @created     : 08/09/2021
 * @filename    : window.h
 */

#ifndef WINDOW_H
#define WINDOW_H

#include <gtk/gtk.h>


gboolean destroy_handler(GtkWidget* window, GtkApplication* alphabet);

void on_activate(GtkApplication* alphabet);

extern int window_run(int argc, char** argv);

#endif

// vim:ft=c

