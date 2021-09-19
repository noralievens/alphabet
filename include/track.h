/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @file        : track.h
 */

#ifndef TRACK_H
#define TRACK_H

#include "config.h"

typedef struct Track {
    char* name;
    char* path;
    double length;
    char* duration;
    int offset;
    double lufs;
    double peak;
    char* artist;
    char* album;
    char* date;
    char* format;
    char* sample_rate;
} Track;

/**
 * Constructor
 *
 * create new track based on path and name
 * name will be overwritten by the file TITLE tag of it exists
 *
 * @param path absolute path of the file
 * @param name displayed name of the file
 */
extern Track* track_new(const char* name, const char* path);

/**
 * Free all resources
 *
 * @param this the track object
 */
extern void track_free(Track* this);

/**
 * Print all track properties
 */
extern void track_print(Track* this);

#endif

// vim:ft=c

