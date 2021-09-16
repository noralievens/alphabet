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

extern void tracklist_remove_selected(Tracklist* this);

extern void tracklist_add(Tracklist* this, Track* track);

extern void tracklist_add_file(Tracklist* this, GFile* file);

extern void tracklist_add_uri(Tracklist* this, const char* path);

extern void tracklist_init(Tracklist* this);

extern Tracklist* tracklist_new(Player* player);

#endif

// vim:ft=c

