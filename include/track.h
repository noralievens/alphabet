/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @created     : 08/09/2021
 * @filename    : track.h
 */

#ifndef TRACK_H
#define TRACK_H

#include <gtk/gtk.h>

typedef struct Track {
    GFile* file;
    gchar* name;
    gchar* uri;
    gdouble length;
    gint offset;
    gdouble gain;
    /* GtkWidget* box; */
    /* GtkWidget* label; */
} Track;

extern Track* track_new(GFile* file);

extern void track_free(Track* this);

extern void track_print(Track* this);

#endif

// vim:ft=c

