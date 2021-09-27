/**
 * @author      Arno Lievens (arnolievens@gmail.com)
 * @date        08/09/2021
 * @file        tracklist.h
 * @brief       track storage and file-manager-like widget
 * @copyright   Copyright (c) 2021 Arno Lievens
 */

#ifndef TRACK_LIST_H
#define TRACK_LIST_H

#include <gtk/gtk.h>

#include "player.h"

/**
 * Columns of the treeview widget
 */
typedef enum TracklistColumn{
    TRACKLIST_COLUMN_NAME,
    TRACKLIST_COLUMN_LUFS,
    TRACKLIST_COLUMN_PEAK,
    TRACKLIST_COLUMN_DURATION,
    TRACKLIST_COLUMN_DATA,
    TRACKLIST_COLUMNS
} TracklistColum;

/**
 * List of target entries for DND
 *
 * used to create table and for info parameter in sig-handlers
 */
typedef enum TracklistEntry {
    TRACKLIST_ENTRY_ROW,
    TRACKLIST_ENTRY_STR,
    TRACKLIST_ENTRY_WAV,
    TRACKLIST_ENTRY_TOT
} TracklistEntry ;

/**
 * Tracklist object
 *
 * storage of tracks as well as treeview widget
 */
typedef struct {
    GtkListStore* list;         /**< data structure of the tree */
    GtkTreeView* tree;          /**< gui widget (file-manager-like) */
    Player* player;             /**< reference to the player object */
    gdouble min_lufs;           /**< min val of all track.lugfs */
    GThreadPool* load_thread;   /**< thread pool for loading tracks */
} Tracklist;

/**
 * Constructor
 *
 * the newly created tracklist only creates the liststore objects
 * because the "open" signal is emitted before the widgets are created
 * call init to create the actual tree
 *
 * @param player reference to the player object used to playstart selected row
 * @return Tracklist newly created object or NULL when failed to load threadpool
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
 * insert track after TreePath or append to the list (NULL)
 *
 * all Tracks will be free-ed by tracklist
 * @param this tracklist object
 * @param track the track to be added
 * @param after insert the new track after this TreePath or NULL to append
 */
extern void tracklist_add_track(Tracklist* this, Track* track, GtkTreePath* after, GtkTreeViewDropPosition pos);

/**
 * Append a file as a track to the tracklist
 *
 * add track asynchronously
 * a new Track is allocated and stored in the list
 * all Tracks can be free-ed by tracklist_free
 * track will be appended to the list
 * file will be free-ed when async loader has finished
 *
 * @param this tracklist object
 * @param file file to be added
 */
extern void tracklist_append_file(Tracklist* this, GFile* file);

/**
 * Insert a file as a track to the tracklist
 *
 * add track asynchronously
 * a new Track is allocated and stored in the list
 * all Tracks can be free-ed by tracklist_free
 * track will be inserted in the list before or after (pos) given row (path)
 * file will be free-ed when async loader has finished
 *
 * @param this tracklist object
 * @param file file to be added
 * @param path new track will be inserted before or after path
 * @param pos insert before (GTK_TREE_VIEW_DROP_POSITION_BEFORE) or (..._AFTER)
 */
extern void tracklist_insert_file(Tracklist* this, GFile* file, GtkTreePath* path, GtkTreeViewDropPosition pos);

/**
 * Remove the currently selected (in treeview) row
 *
 * @param this tracklist object
 */
extern void tracklist_remove_selected(Tracklist* this);

/**
 * Create a new track from file
 *
 * tracklist_add_file calls this function in a threadpool
 * return track free-ed by tracklist_free or track_free
 *
 * @param file the file used to create a track
 * @return the newly created track or NULL
 */
extern Track* tracklist_file_to_track(Tracklist* this, GFile* file);

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
