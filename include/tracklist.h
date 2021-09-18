/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @file        : tracklist.h
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
    GtkListStore* list;         /**< data structure of the tree */
    GtkWidget* tree;            /**< gui widget (file-manager-like) */
    Player* player;             /**< reference to the player object */
} Tracklist;

/**
 * Constructor
 *
 * the newly created tracklist only creates the liststore objects
 * because the "open" signal is emitted before the widgets are created
 * call init to create the actual tree
 *
 * @param player reference to the player object used to playstart selected row
 * @return Tracklist newly create object
 */
extern Tracklist* tracklist_new(Player* player);

/**
 * Generate the gui elements of the tree
 *
 * @param this tracklist object
 */
extern void tracklist_init(Tracklist* this);

/**
 * Add a track to the tracklist
 *
 * all Tracks will be free-ed by tracklist
 * @param this tracklist object
 * @param track the track to be added
 */
extern void tracklist_add_track(Tracklist* this, Track* track);

/**
 * Add a file as a track to the tracklist
 *
 * validate file exists and is an audio/\* file
 * a new Track is allocated and stored in the list
 * all Tracks will be free-ed by tracklist
 *
 * @param this tracklist object
 * @param file file to be added
 */
extern void tracklist_add_file(Tracklist* this, GFile* file);

/**
 * Remove the currently selected (in treeview) row
 *
 * @param this tracklist object
 */
extern void tracklist_remove_selected(Tracklist* this);

/**
 * Free all resources
 *
 * this includes:
 *      all tracks
 *      treeview
 *      liststore
 *
 * @param this tracklist object
 */
extern void tracklist_free(Tracklist* this);

#endif

// vim:ft=c

