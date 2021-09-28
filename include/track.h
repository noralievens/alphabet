/**
 * @author      Arno Lievens (arnolievens@gmail.com)
 * @date        08/09/2021
 * @file        track.h
 * @brief       track class
 * @copyright   Copyright (c) 2021 Arno Lievens
 */

#ifndef TRACK_H
#define TRACK_H

#include "config.h"

/**
 * Track
 *
 * represents an audio file on disk
 */
typedef struct Track {
    char* name;             /**< file basename or TITLE/NAME tag */
    char* path;             /**< file absolute path */
    double length;          /**< estimated length (samplerate * samples */
    int offset;             /**< TODO: auto*align */
    double lufs;            /**< averge loudness level as calculated by r128 */
    double peak;            /**< true peak level */
    char* artist;           /**< ARTIST tag if present or NULL */
    char* album;            /**< ALBUM tag if present or NULL */
    char* date;             /**< DATE tag if present or NULL */
    char* format;           /**< TODO: audio file format eg flac, mp3, wav */
    char* sample_rate;      /**< sample rate eg 44100 96000 */
} Track;

/**
 * Constructor
 *
 * create new track based on path and name
 * name will be overwritten by the file TITLE or NAME tag of it exists
 *
 * @param path absolute path of the file
 * @param name displayed name of the file
 * @return the newly created Track or NULL when failed
 */
extern Track* track_new(const char* name, const char* path);

/**
 * Print all track properties
 *
 * @param this the track object
 */
extern void track_print(Track* this);
/**
 * Free all resources
 *
 * @param this the track object
 */
extern void track_free(Track* this);

#endif
