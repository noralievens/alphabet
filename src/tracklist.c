/**
 * @author      Arno Lievens (arnolievens@gmail.com)
 * @date        08/09/2021
 * @file        tracklist.c
 * @brief       track storage and file-manager-like widget
 * @copyright   Copyright (c) 2021 Arno Lievens
 */

#include <assert.h>
#include <errno.h>
#include <gtk/gtk.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/config.h"
#include "../include/player.h"
#include "../include/track.h"
#include "../include/tracklist.h"

/* static gboolean (*GtkTreeViewSearchEqualFunc) (GtkTreeModel *model, gint column, const gchar *key, GtkTreeIter *iter, gpointer search_data) */

/**
* Play the selected song in player
*
* @param this
* @param selection the selected track (max 1)
*/
static void tracklist_on_selection_changed(Tracklist* this, GtkTreeSelection* selection);

/**
 * function used by load_thread threadpool to async load tracks
 *
 * @param data file closure from thread_pool_push call
 * @param user_data tracklist object closure from thread_pool_new
 */
static void tracklist_load_async(gpointer data, gpointer user_data);


/*******************************************************************************
 * extern functions
 */

typedef enum TRACKLIST_TABLE_ENTRY {
    ENTRY_ROW,
    ENTRY_STR,
    ENTRY_WAV,
    ENTRY_TOT
} TRACKLIST_TABLE_ENTRY;


GtkTargetEntry entries[ENTRY_TOT] = {
    [ENTRY_ROW] = {"GTK_TREE_MODEL_ROW", GTK_TARGET_SAME_WIDGET, ENTRY_ROW},
    [ENTRY_STR] = {"STRING", GTK_TARGET_OTHER_APP, ENTRY_STR},
    [ENTRY_WAV] = {"audio/x-wav", GTK_TARGET_OTHER_APP, ENTRY_WAV},
    /* {"text/uri-list", 0, 3} */
};

Tracklist* tracklist_new(Player* player)
{
    GError* err = NULL;
    Tracklist* this = malloc(sizeof(Tracklist));
    this->player = player;

    this->list = gtk_list_store_new(
            TRACKLIST_COLUMNS,
            G_TYPE_STRING,
            G_TYPE_STRING,
            G_TYPE_STRING,
            G_TYPE_STRING,
            G_TYPE_POINTER);

    /* create threadpool for async loading of files */
    /* files can be added: g_thread_pool_push(this->load_thread, file, &err); */
    /* FIXME only works for MAX THREADS = -1 - segfult for limited threads*/
    this->load_thread = g_thread_pool_new(
            tracklist_load_async, this, -1, FALSE, &err);

    if (err) {
        g_printerr("%s\n", err->message);
        return NULL;
    }
    return this;
}








void drag_begin (GtkTreeView *tree, GdkDragContext *ctx, Tracklist* this)
{
}

gboolean drag_motion(GtkTreeView* tree, GdkDragContext* ctx, gint x, gint y, guint time, gpointer user_data)
{
    GdkAtom target_atom = gtk_drag_dest_find_target(GTK_WIDGET(tree), ctx, NULL);

    if (target_atom != GDK_NONE) {
        GtkTreePath* path;
        GtkTreeViewDropPosition pos;
        gtk_tree_view_get_dest_row_at_pos(tree, x, y, &path, &pos);
        gtk_tree_view_set_drag_dest_row(tree, path, pos);

        GdkDragAction action = gdk_drag_context_get_suggested_action(ctx);
        gdk_drag_status(ctx, action, time);
        return TRUE;
    }
    gdk_drag_status(ctx, 0, time);

    return FALSE;
}

gboolean drag_drop(GtkTreeView* tree, GdkDragContext* ctx, gint x, gint y, guint time, gpointer user_data)
{
    GdkAtom target_atom = gtk_drag_dest_find_target(GTK_WIDGET(tree), ctx, NULL);

    if (target_atom != GDK_NONE) {

        /* GtkTreePath* dest; */
        /* GtkTreeViewDropPosition pos; */
        /*  */
        /* gtk_tree_view_get_dest_row_at_pos(tree, x, y, &dest, &pos); */

        /* gtk_drag_get_data(tree, ctx, target_atom, time); */
        return FALSE;
    }
    return FALSE;
}

void
drag_data_get (GtkTreeView* tree, GdkDragContext* ctx, GtkSelectionData* selection, guint info, guint time, gpointer data)
{
}


static void drag_data_received(GtkTreeView* tree, GdkDragContext *ctx, gint x, gint y, GtkSelectionData *selection, guint info, guint32 time, Tracklist* this)
{
    gint wx, wy;

    switch (info) {
        case ENTRY_STR: {
            GtkTreePath* path;
            GtkTreeViewDropPosition pos;
            gtk_tree_view_get_dest_row_at_pos(tree, x, y, &path, &pos);

            /* tfw why \r ?? */
            const gchar* delim = "\r\n";
            const guchar* uris = gtk_selection_data_get_data(selection);
            char* str = g_strdup((const char*)uris);
            char* uri;
            uri = strtok(str, delim);
            do {
                GFile* file = g_file_new_for_uri(uri);
                tracklist_insert_file(this, file, path, pos);
            } while ((uri = strtok(NULL, delim)));

            gtk_drag_finish(ctx, TRUE, TRUE, time);
            g_free(str);
            break;
        }

        case ENTRY_ROW: {
            break;
        }

        default: printf("DEBUG: INVALID GDK ATOM\n"); break;
    }

    gtk_drag_finish(ctx, FALSE, FALSE, time);
}

void drag_leave(GtkWidget* tree, GdkDragContext* ctx, guint time, gpointer user_data)
{
    printf("leave\n");
}

void drag_data_delete(GtkWidget* tree, GdkDragContext* ctx, gpointer user_data)
{
    /* printf("drag-delete\n"); */
}

gboolean drag_data_failed(GtkWidget *tree, GdkDragContext *ctx, GtkDragResult result, gpointer user_data)
{
    printf("drag-failed: %d\n", result);
    return FALSE;
}









void tracklist_init(Tracklist* this)
{
    GtkTreeSelection* selection;
    GtkTreeViewColumn* column;
    GtkCellRenderer* cellrenderer;
    int id;

    this->tree = GTK_TREE_VIEW(
            gtk_tree_view_new_with_model(GTK_TREE_MODEL(this->list)));

    gtk_tree_view_set_reorderable(this->tree, TRUE);
    gtk_tree_view_set_enable_search(this->tree, FALSE);

    selection = gtk_tree_view_get_selection(this->tree);
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_BROWSE);

    g_signal_connect_swapped(
            selection,
            "changed",
            G_CALLBACK(tracklist_on_selection_changed),
            this);

    id = TRACKLIST_COLUMN_NAME;
    column = gtk_tree_view_column_new();
    gtk_tree_view_append_column(this->tree, column);
    cellrenderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, cellrenderer, FALSE);
    gtk_tree_view_column_set_resizable(column, FALSE);
    gtk_tree_view_column_set_clickable(column, TRUE);
    gtk_tree_view_column_add_attribute(column, cellrenderer, "text", id);
    gtk_tree_view_column_set_title(column, "Track");
    gtk_tree_view_column_set_expand(column, TRUE);
    g_object_set(G_OBJECT(cellrenderer), "ellipsize", PANGO_ELLIPSIZE_END, NULL);
    /* gtk_tree_view_column_set_sort_column_id(column, id); */
    gtk_tree_view_set_search_column(this->tree, TRACKLIST_COLUMN_NAME);

    id = TRACKLIST_COLUMN_LUFS;
    column = gtk_tree_view_column_new();
    gtk_tree_view_append_column(this->tree, column);
    gtk_tree_view_column_set_alignment(column, 0.5);
    cellrenderer = gtk_cell_renderer_text_new();
    gtk_cell_renderer_set_alignment(cellrenderer, 0.5, 0.0);
    gtk_tree_view_column_pack_start(column, cellrenderer, FALSE);
    gtk_tree_view_column_set_resizable(column, FALSE);
    gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_column_set_clickable(column, TRUE);
    gtk_tree_view_column_add_attribute(column, cellrenderer, "text", id);
    gtk_tree_view_column_set_title(column, "LUFs");
    gtk_tree_view_column_set_expand(column, FALSE);
    /* gtk_tree_view_column_set_sort_column_id(column, id); */

    id = TRACKLIST_COLUMN_PEAK;
    column = gtk_tree_view_column_new();
    gtk_tree_view_append_column(this->tree, column);
    gtk_tree_view_column_set_alignment(column, 0.5);
    cellrenderer = gtk_cell_renderer_text_new();
    gtk_cell_renderer_set_alignment(cellrenderer, 0.5, 0.0);
    gtk_tree_view_column_pack_start(column, cellrenderer, FALSE);
    gtk_tree_view_column_set_resizable(column, FALSE);
    gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_column_set_clickable(column, TRUE);
    gtk_tree_view_column_add_attribute(column, cellrenderer, "text", id);
    gtk_tree_view_column_set_title(column, "Peak");
    gtk_tree_view_column_set_expand(column, FALSE);
    /* gtk_tree_view_column_set_sort_column_id(column, id); */

    id = TRACKLIST_COLUMN_DURATION;
    column = gtk_tree_view_column_new();
    gtk_tree_view_append_column(this->tree, column);
    gtk_tree_view_column_set_alignment(column, 0.5);
    cellrenderer = gtk_cell_renderer_text_new();
    gtk_cell_renderer_set_alignment(cellrenderer, 0.5, 0.0);
    gtk_tree_view_column_pack_start(column, cellrenderer, FALSE);
    gtk_tree_view_column_set_resizable(column, FALSE);
    gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_column_set_clickable(column, TRUE);
    gtk_tree_view_column_add_attribute(column, cellrenderer, "text", id);
    gtk_tree_view_column_set_title(column, "Duration");
    gtk_tree_view_column_set_expand(column, FALSE);
    /* gtk_tree_view_column_set_sort_column_id(column, id); */

    GtkTargetList* target_list = gtk_target_list_new(NULL, 0);
    gdk_atom_intern(entries[ENTRY_STR].target, TRUE);
    gdk_atom_intern(entries[ENTRY_ROW].target, TRUE);
    gdk_atom_intern(entries[ENTRY_WAV].target, TRUE);
    gtk_target_list_add_table(target_list, entries, ENTRY_TOT);

    gtk_tree_view_enable_model_drag_source(this->tree, GDK_BUTTON1_MASK, entries, ENTRY_ROW+1, GDK_ACTION_MOVE);
    gtk_tree_view_enable_model_drag_dest(this->tree, entries, ENTRY_TOT, GDK_ACTION_MOVE);

    /* g_signal_connect(this->tree, "drag-begin", G_CALLBACK(drag_begin), this); */
    g_signal_connect(this->tree, "drag-motion", G_CALLBACK(drag_motion), this);
    g_signal_connect(this->tree, "drag-drop", G_CALLBACK(drag_drop), this);
    g_signal_connect(this->tree, "drag-data-get", G_CALLBACK(drag_data_get), this);
    g_signal_connect(this->tree, "drag-data-received", G_CALLBACK(drag_data_received), this);
    g_signal_connect(this->tree, "drag-failed", G_CALLBACK(drag_data_failed), this);
    g_signal_connect(this->tree, "drag-data-delete", G_CALLBACK(drag_data_delete), this);
    /* g_signal_connect(this->tree, "drag-leave", G_CALLBACK(drag_leave), this); */

    gtk_widget_show_all(GTK_WIDGET(this->tree));
}

void tracklist_add_track(Tracklist* this, Track* track, GtkTreePath* path, GtkTreeViewDropPosition pos)
{
    if (!track) return;
    GtkTreeIter iter, prev;

    gchar lufs[7];
    g_snprintf(lufs, G_N_ELEMENTS(lufs), "%.2f", track->lufs);

    gchar peak[7];
    g_snprintf(peak, G_N_ELEMENTS(peak), "%.2f", track->peak);

    gchar duration[10];
    dtoduration(duration, track->length);

    if (path && gtk_tree_model_get_iter(GTK_TREE_MODEL(this->list), &prev, path)) {
        switch (pos) {
            case GTK_TREE_VIEW_DROP_BEFORE:
            case GTK_TREE_VIEW_DROP_INTO_OR_BEFORE:
                gtk_list_store_insert_before(this->list, &iter, &prev);
                break;

            case GTK_TREE_VIEW_DROP_AFTER:
            case GTK_TREE_VIEW_DROP_INTO_OR_AFTER:
            default:
                gtk_list_store_insert_after(this->list, &iter, &prev);
        }
    } else {
        gtk_tree_path_free(path);
        gtk_list_store_append(this->list, &iter);
    }
    gtk_list_store_set(
            this->list, &iter,
            TRACKLIST_COLUMN_NAME, track->name,
            TRACKLIST_COLUMN_LUFS, lufs,
            TRACKLIST_COLUMN_PEAK, peak,
            TRACKLIST_COLUMN_DURATION, duration,
            TRACKLIST_COLUMN_DATA, track,
            -1);

    this->min_lufs = MIN(this->min_lufs, track->lufs);
    this->player->min_lufs = this->min_lufs;

}

Track* tracklist_file_to_track(UNUSED Tracklist* this, GFile* file)
{
    if (!G_IS_FILE(file)) return NULL;
    const char* type;
    char* name, * path;
    GError *err = NULL;
    GFileInfo* info;

        printf("path: %s\n", g_file_get_path(file));
        printf("name: %s\n", g_file_get_basename(file));
    info = g_file_query_info(file, G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE,
            G_FILE_QUERY_INFO_NONE, NULL, &err);

    /* file-open errors - report and cancel */
    if (err) {
        g_printerr("%s\n", err->message);
        g_error_free(err);
        return NULL;
    }

    name = g_file_get_basename(file);
    path = g_file_get_path(file);

    /* mime type not readable ?? */
    if (!(type = g_file_info_get_content_type(info))) {
        g_printerr("Error getting mimetype for file \"%s\"\n", path);
        goto fail;
    }

    /* invalid file*/
    if (!g_strstr_len(type, -1, "audio")) {
        g_printerr("Error loading file \"%s\": Not and audio file\n", path);
        goto fail;
    }

    Track* track = track_new(name, path);
    g_object_unref(info);
    g_free(name);
    g_free(path);
    return track;

fail:
    g_object_unref(info);
    g_free(name);
    g_free(path);
    return NULL;
}

LoadFileData* loadfiledata_new(GFile* file, GtkTreePath* path, GtkTreeViewDropPosition pos)
{
    LoadFileData* this = malloc(sizeof(LoadFileData));
    this->file = file;
    this->position = path;
    this->drop_position = pos;
    return this;
}

void loadfiledata_free(LoadFileData* this)
{
    free(this);
}

void tracklist_insert_file(Tracklist* this, GFile* file, GtkTreePath* path, GtkTreeViewDropPosition pos)
{
    GError* err = NULL;
    LoadFileData* data = loadfiledata_new(file, path, pos);
    g_thread_pool_push(this->load_thread, data, &err);
    if (err) {
        g_printerr("%s\n", err->message);
    }
}

void tracklist_append_file(Tracklist* this, GFile* file)
{
    tracklist_insert_file(this, file, NULL, 0);
}

void tracklist_remove_selected(Tracklist* this)
{
    GtkTreeSelection* selection = gtk_tree_view_get_selection(this->tree);
    Track* track;
    GtkTreeIter iter;
    GtkTreeModel* model = GTK_TREE_MODEL(this->list);
    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) return;
    gtk_tree_model_get(model, &iter, TRACKLIST_COLUMN_DATA, &track, -1);
    gtk_list_store_remove(this->list, &iter);
    track_free(track);
}

void tracklist_free(Tracklist* this)
{
    if (!this) return;

    GtkTreeModel* model = GTK_TREE_MODEL(this->list);
    Track* track;
    GtkTreeIter iter;
    GtkTreeSelection* selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(this->tree));

    g_signal_handlers_disconnect_by_data(selection, this);

    gtk_tree_selection_get_selected(selection, &model, &iter);

    while (gtk_list_store_iter_is_valid(this->list, &iter)) {
        gtk_tree_model_get(model, &iter, TRACKLIST_COLUMN_DATA, &track, -1);
        track_free(track);
        gtk_list_store_remove(this->list, &iter);
    }

    this->player->current = NULL;

    gtk_widget_destroy(GTK_WIDGET(this->tree));
    g_object_unref(this->list);
    g_free(this);
}


/*******************************************************************************
 * static functions
 *
 */

void tracklist_on_selection_changed(Tracklist* this, GtkTreeSelection* selection)
{
    Track* track;
    GtkTreeIter iter;
    /* TODO don't need to set model, its set in ..._get_selection */
    GtkTreeModel* model = GTK_TREE_MODEL(this->list);

    gtk_tree_selection_get_selected(selection, &model, &iter);
    if (!gtk_list_store_iter_is_valid(this->list, &iter)) return;
    gtk_tree_model_get(model, &iter, TRACKLIST_COLUMN_DATA, &track, -1);

    gdouble position = 0.0;
    if (this->player->marker) position = this->player->marker;
    else if (!this->player->rtn) position = player_update(this->player);
    player_load_track(this->player, track, position);
}

void tracklist_load_async(gpointer data, gpointer user_data)
{
    Tracklist* this = user_data;
    LoadFileData* loadfile = data;
    Track* track = tracklist_file_to_track(this, loadfile->file);
    /* this shouldn't be wrapped in g_idle_ad ?? */
    if (track) tracklist_add_track(this, track, loadfile->position, loadfile->drop_position);
    loadfiledata_free(data);
}
