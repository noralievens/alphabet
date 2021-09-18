/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @file        : track.h
 */

#ifndef TRACK_H
#define TRACK_H

#include <gtk/gtk.h>

typedef struct Track {
    char* name;
    char* uri;
    double length;
    char* duration;
    int offset;
    double gain;
} Track;

extern Track* track_new(const char* name, const char* file);

extern void track_free(Track* this);

extern void track_print(Track* this);

#endif

// vim:ft=c

