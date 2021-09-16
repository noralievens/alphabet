/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @created     : 16/09/2021
 * @filename    : tree.c
 */

#include <assert.h>
#include <errno.h>
#include <gtk/gtk.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "../include/config.h"
#include "../include/track.h"
#include "../include/player.h"

#include "../include/tracklist.h"

/* static gboolean (*GtkTreeViewSearchEqualFunc) (GtkTreeModel *model, gint column, const gchar *key, GtkTreeIter *iter, gpointer search_data) */

static void tracklist_on_selection_changed(Tracklist* this, GtkTreeSelection* selection)
{
    Track* track;
    GtkTreeIter iter;
    GtkTreeModel* model = GTK_TREE_MODEL(this->list);

    gtk_tree_selection_get_selected(selection, &model, &iter);
    if (!gtk_list_store_iter_is_valid(this->list, &iter)) return;
    gtk_tree_model_get(model, &iter, TRACKLIST_COLUMN_DATA, &track, -1);

    gdouble position = 0.0;
    if (this->player->marker) position = this->player->marker;
    else if (!this->player->rtn) position = player_update(this->player);
    player_load_track(this->player, track, position);
}

void tracklist_remove_selected(Tracklist* this)
{
    GtkTreeSelection* selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(this->tree));
    Track* track;
    GtkTreeIter iter;
    GtkTreeModel* model = GTK_TREE_MODEL(this->list);
    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) return;
    gtk_tree_model_get(model, &iter, TRACKLIST_COLUMN_DATA, &track, -1);
    gtk_list_store_remove(this->list, &iter);
    track_free(track);
}

void tracklist_add(Tracklist* this, Track* track)
{
    GtkTreeIter iter;
    gtk_list_store_append(this->list, &iter);
    gtk_list_store_set(
            this->list, &iter,
            TRACKLIST_COLUMN_NAME, track->name,
            TRACKLIST_COLUMN_DURATION, track->duration,
            TRACKLIST_COLUMN_DATA, track,
            -1);
}

void tracklist_add_file(Tracklist* this, GFile* file)
{
    /* TODO validate audio file ! */
    Track* track = track_new(g_file_get_basename(file), g_file_get_path(file));
    tracklist_add(this, track);
}

void tracklist_add_uri(Tracklist* this, const char* path)
{
    /* TODO validate audio file ! */
    Track* track = track_new(path, path);
    tracklist_add(this, track);
}

Tracklist* tracklist_new(Player* player)
{
    Tracklist* this = malloc(sizeof(Tracklist));
    this->player = player;

    this->list = gtk_list_store_new(
            TRACKLIST_COLUMNS,
            G_TYPE_STRING,
            G_TYPE_STRING,
            G_TYPE_POINTER);
    return this;
}

void tracklist_init(Tracklist* this)
{
    GtkTreeSelection* selection;
    GtkTreeViewColumn* column;
    GtkCellRenderer* cellrenderer;
    int id;

    this->tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(this->list));
    gtk_tree_view_set_reorderable(GTK_TREE_VIEW(this->tree), TRUE);
    gtk_tree_view_set_enable_search(GTK_TREE_VIEW(this->tree), FALSE);

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(this->tree));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_BROWSE);
    g_signal_connect_swapped(selection, "changed", G_CALLBACK(tracklist_on_selection_changed), this);

    id = 0;
    column = gtk_tree_view_column_new();
    gtk_tree_view_append_column(GTK_TREE_VIEW(this->tree), column);
    cellrenderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, cellrenderer, FALSE);
    gtk_tree_view_column_set_resizable(column, FALSE);
    /* gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED); */
    gtk_tree_view_column_set_clickable(column, TRUE);
    gtk_tree_view_column_add_attribute(column, cellrenderer, "text", id);
    gtk_tree_view_column_set_title(column, "Track");
    gtk_tree_view_column_set_expand(column, TRUE);
    g_object_set(G_OBJECT(cellrenderer), "ellipsize", PANGO_ELLIPSIZE_END, NULL);
    gtk_tree_view_column_set_sort_column_id(column, id);
    gtk_tree_view_set_search_column(GTK_TREE_VIEW(this->tree), TRACKLIST_COLUMN_NAME);

    id = 1;
    column = gtk_tree_view_column_new();
    gtk_tree_view_append_column(GTK_TREE_VIEW(this->tree), column);
    cellrenderer = gtk_cell_renderer_text_new();
    gtk_cell_renderer_set_alignment(cellrenderer, 1.0, 0.0);
    /* gtk_cell_renderer_set_padding(cellrenderer, TEXT_PADDING, 0); */
    gtk_tree_view_column_pack_start(column, cellrenderer, FALSE);
    gtk_tree_view_column_set_resizable(column, FALSE);
    gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_column_set_clickable(column, TRUE);
    gtk_tree_view_column_add_attribute(column, cellrenderer, "text", id);
    gtk_tree_view_column_set_title(column, "Duration");
    gtk_tree_view_column_set_expand(column, FALSE);
    g_object_set(G_OBJECT(cellrenderer), "ellipsize", PANGO_ELLIPSIZE_END, NULL);
    gtk_tree_view_column_set_sort_column_id(column, id);

    gtk_widget_show_all(this->tree);
}





