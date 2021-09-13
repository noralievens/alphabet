/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @created     : 08/09/2021
 * @filename    : track.h
 */

#ifndef TRACK_H
#define TRACK_H

#include <gtk/gtk.h>

typedef struct Track {
    char* file;
    char* name;
    char* uri;
    double length;
    char* duration;
    int offset;
    double gain;
    /* GtkWidget* box; */
    /* GtkWidget* label; */
} Track;

extern Track* track_new(const char* name, const char* file);

extern void track_free(Track* this);

extern void track_print(Track* this);

#endif

// vim:ft=c

