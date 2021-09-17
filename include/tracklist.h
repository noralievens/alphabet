/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @created     : 16/09/2021
 * @filename    : tree.h
 */

#ifndef TRACK_LIST_H
#define TRACK_LIST_H

#include <gtk/gtk.h>

#include "player.h"

enum {
    TRACKLIST_COLUMN_NAME,
    TRACKLIST_COLUMN_DURATION,
    TRACKLIST_COLUMN_DATA,
    TRACKLIST_COLUMNS
};

typedef struct {
    GtkListStore* list;
    GtkWidget* tree;
    Player* player;
} Tracklist;

extern Tracklist* tracklist_new(Player* player);

extern void tracklist_add_track(Tracklist* this, Track* track);

/**
 * Add a file as a track to the tracklist
 *
 * validate file exists and is an audio/\* file
 *
 * @param this tracklist object
 * @file file to be added
 */
extern void tracklist_add_file(Tracklist* this, GFile* file);

extern void tracklist_init(Tracklist* this);

extern void tracklist_remove_selected(Tracklist* this);

extern void tracklist_free(Tracklist* this);

#endif

// vim:ft=c

