/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @created     : 08/09/2021
 * @filename    : window.c
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

#include "../include/track.h"
#include "../include/player.h"

#include "../include/window.h"

#define UNUSED __attribute__((unused))

mpv_handle* mpv;
GtkListStore* list;

void on_activate_row(UNUSED GtkTreeView* tree_view, GtkTreePath *path, UNUSED GtkTreeViewColumn *column, GtkTreeModel* model)
{
    Track* track;
    GtkTreeIter iter;
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(model, &iter, 1, &track, -1);

    gdouble position = player_get_position(mpv);
    player_load_track(mpv, track, position);
    /* player_set_position(mpv, position); */
}

void on_open(GApplication *alphabet, GFile **files, gint n_files, UNUSED const gchar *hint)
{
    list = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
    GtkTreeIter iter;

    for (gint i = 0; i < n_files; i++) {
        Track* track = track_new(files[i]);

        track_print(track);
        gtk_list_store_append(list, &iter);
        gtk_list_store_set(list, &iter,
                0, track->name,
                1, track,
                -1);
    }

    on_activate(GTK_APPLICATION(alphabet));
}

gboolean destroy_handler(GtkWidget* window, GtkApplication* alphabet)
{
    gtk_window_close(GTK_WINDOW(window));
    g_application_quit(G_APPLICATION(alphabet));
    return TRUE;
}

void on_activate(GtkApplication* alphabet)
{
    printf("activate\n");
    GtkWidget* window, * box;
    window = gtk_application_window_new(alphabet);
    gtk_window_set_title(GTK_WINDOW(window), "alphabet");
    gtk_container_set_border_width(GTK_CONTAINER(window), 0);
    gtk_window_set_default_size(GTK_WINDOW(window), 960, 540);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    g_signal_connect(window, "destroy", G_CALLBACK(destroy_handler), alphabet);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), box);

    GtkWidget* tree;
    GtkTreeSelection* selection;
    GtkTreeViewColumn* column;
    GtkCellRenderer* cellrenderer;
    int id;

    tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list));
    gtk_tree_view_set_reorderable(GTK_TREE_VIEW(tree), TRUE);
    g_signal_connect(tree, "row-activated",
            G_CALLBACK(on_activate_row), GTK_TREE_MODEL(list));

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);

    gtk_box_pack_start(GTK_BOX(box), tree, FALSE, FALSE, 0);

    id = 0;
    column = gtk_tree_view_column_new();
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);
    cellrenderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, cellrenderer, FALSE);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED);
    gtk_tree_view_column_set_clickable(column, TRUE);
    gtk_tree_view_column_add_attribute(column, cellrenderer, "text", id);
    gtk_tree_view_column_set_title(column, "Name");
    gtk_tree_view_column_set_expand(column, TRUE);
    g_object_set(G_OBJECT(cellrenderer), "ellipsize", PANGO_ELLIPSIZE_END, NULL);
    gtk_tree_view_column_set_sort_column_id(column, id);
    /* g_signal_connect(column, "clicked", G_CALLBACK(on_clicked_column), tree); */


    gtk_widget_show_all(window);
}

int window_run(int argc, char** argv)
{
    int status;
    GtkApplication* alphabet;

    mpv = player_init();
    if (!mpv) exit(EXIT_FAILURE);

    alphabet = gtk_application_new("org.gtk.alphabet", G_APPLICATION_HANDLES_OPEN);
    g_signal_connect(alphabet, "open", G_CALLBACK(on_open), NULL);
    g_signal_connect(alphabet, "activate", G_CALLBACK(on_activate), NULL);

    status = g_application_run(G_APPLICATION(alphabet), argc, argv);

    /* g_object_unref(alphabet); */
    g_application_quit(G_APPLICATION(alphabet));
    mpv_terminate_destroy(mpv);
    return status;
}
