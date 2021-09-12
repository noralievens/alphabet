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
#include "../include/timeline.h"
#include "../include/transport.h"

#include "../include/window.h"

#define UNUSED __attribute__((unused))

gboolean m;
GtkListStore* list;

Player* player;
Transport* transport;



void on_selection_changed(GtkTreeSelection* selection, GtkTreeModel* model)
{
    Track* track;
    GtkTreeIter iter;
    gtk_tree_selection_get_selected(selection, &model, &iter);
    gtk_tree_model_get(model, &iter, 1, &track, -1);

    gdouble position = 0.0;
    if (player->marker) position = player->marker;
    else if (!player->rtn) position = player_update(player);
    player_load_track(player, track, position);
}

void remove_selected(GtkTreeSelection* selection)
{
    Track* track;
    GtkTreeIter iter;
    GtkTreeModel* model = GTK_TREE_MODEL(list);
    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) return;
    gtk_tree_model_get(model, &iter, 1, &track, -1);
    gtk_list_store_remove(list, &iter);
    track_free(track);
}

void add_track(GFile* file)
{
    /* TODO validate audio file ! */
    Track* track = track_new(file);
    GtkTreeIter iter;
    gtk_list_store_append(list, &iter);
    gtk_list_store_set(list, &iter, 0, track->name, 1, track, -1);
}

void on_open(GApplication *alphabet, GFile **files, gint n_files, UNUSED const gchar *hint)
{
    printf("n: %i\n", n_files);
    for (gint i = 0; i < n_files; i++) {
        add_track(files[i]);
    }
    on_activate(GTK_APPLICATION(alphabet));
}

gboolean destroy_handler(UNUSED GtkWidget* window, UNUSED GtkApplication* alphabet)
{
    printf("destroy_handler\n");
    /* gtk_window_close(GTK_WINDOW(window)); */
    /* g_application_quit(G_APPLICATION(alphabet)); */
    return TRUE;
}

void show_file_chooser(GtkWindow* window)
{
    GtkFileChooserNative *chsr;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;

    chsr = gtk_file_chooser_native_new("Add file", window, action, "_Add", "_Cancel");
    gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(chsr), TRUE);

    if (gtk_native_dialog_run(GTK_NATIVE_DIALOG(chsr)) == GTK_RESPONSE_ACCEPT) {

        GSList* filelist = gtk_file_chooser_get_files(GTK_FILE_CHOOSER(chsr));
        do add_track(filelist->data);
        while ((filelist = filelist->next));

        g_slist_free(filelist);

    }
    g_object_unref(chsr);
}

gboolean keypress_handler(GtkWidget *window, GdkEventKey *event, GtkTreeView* tree)
{
    switch (event->keyval) {

        case GDK_KEY_Delete:
            remove_selected(gtk_tree_view_get_selection(tree));
            return TRUE;

        case (GDK_CONTROL_MASK & GDK_KEY_q):
            gtk_window_close(GTK_WINDOW(window));
            return TRUE;

        case GDK_KEY_n:
            player->rtn = !player->rtn;
            transport_update(transport);
            return TRUE;

        case GDK_KEY_l:
            player_loop(player);
            return TRUE;

        case GDK_KEY_space:
            player_toggle(player);
            return TRUE;

        case GDK_KEY_Left:
            player_seek(player, -1.0);
            return TRUE;

        case GDK_KEY_Right:
            player_seek(player, 1.0);
            return TRUE;

        case GDK_KEY_m:
        case GDK_KEY_KP_Enter:
            player_mark(player);
            return TRUE;

        case GDK_KEY_Return:
            player_goto(player, 0.0);
            player_toggle(player);
            player_mark(player);
            return TRUE;

        default:
            return FALSE;
    }
    return FALSE;
}

void on_clicked_add(UNUSED GtkButton* this, GtkWindow* window)
{
    show_file_chooser(GTK_WINDOW(window));
}

void on_clicked_delete(UNUSED GtkButton* this, GtkTreeView* tree)
{
    remove_selected(gtk_tree_view_get_selection(tree));
}

GtkWidget* tree_new(void)
{
    GtkWidget* tree;
    GtkTreeSelection* selection;
    GtkTreeViewColumn* column;
    GtkCellRenderer* cellrenderer;
    int id;

    tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(list));
    gtk_tree_view_set_reorderable(GTK_TREE_VIEW(tree), TRUE);
    /* g_signal_connect(tree, "row-activated", */
    /*         G_CALLBACK(on_activate_row), GTK_TREE_MODEL(list)); */

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_BROWSE);
    g_signal_connect(selection, "changed", G_CALLBACK(on_selection_changed), GTK_TREE_MODEL(list));

    id = 0;
    column = gtk_tree_view_column_new();
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);
    cellrenderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(column, cellrenderer, FALSE);
    gtk_tree_view_column_set_resizable(column, FALSE);
    /* gtk_tree_view_column_set_sizing(column, GTK_TREE_VIEW_COLUMN_FIXED); */
    gtk_tree_view_column_set_clickable(column, TRUE);
    gtk_tree_view_column_add_attribute(column, cellrenderer, "text", id);
    gtk_tree_view_column_set_title(column, "Tracks");
    gtk_tree_view_column_set_expand(column, TRUE);
    g_object_set(G_OBJECT(cellrenderer), "ellipsize", PANGO_ELLIPSIZE_END, NULL);
    gtk_tree_view_column_set_sort_column_id(column, id);

    return tree;
}


void* update_timeline(UNUSED gpointer data)
{

    /* player_update(player); */
    /* gtk_widget_queue_draw(timeline); */
    return FALSE;
}

void on_activate(GtkApplication* alphabet)
{
    GtkWidget* window, * box, *scrolled;
    GtkWidget* tree, * foo, * button, * timeline;

    window = gtk_application_window_new(alphabet);
    gtk_window_set_title(GTK_WINDOW(window), "alphabet");
    gtk_container_set_border_width(GTK_CONTAINER(window), 0);
    gtk_window_set_default_size(GTK_WINDOW(window), 480, 480);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_widget_show_all(window);

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), box);

    scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(box), scrolled, TRUE, TRUE, GDK_ACTION_DEFAULT);
    /* gtk_scrolled_window_set_propagate_natural_height( GTK_SCROLLED_WINDOW(scrolled), TRUE); */
    /* gtk_scrolled_window_set_propagate_natural_width( GTK_SCROLLED_WINDOW(scrolled), TRUE); */

    tree = tree_new();
    gtk_container_add(GTK_CONTAINER(scrolled), tree);

    foo = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_end(GTK_BOX(box), foo, FALSE, FALSE, 0);

    button = gtk_button_new_with_mnemonic("_Add");
    gtk_box_pack_start(GTK_BOX(foo), button, FALSE, FALSE, 0);
    g_signal_connect(button, "clicked", G_CALLBACK(on_clicked_add), window);

    button = gtk_button_new_with_mnemonic("_Delete");
    gtk_box_pack_start(GTK_BOX(foo), button, FALSE, FALSE, 0);
    g_signal_connect(button, "clicked", G_CALLBACK(on_clicked_delete), tree);
    gtk_widget_show_all(box);

    timeline = timeline_new(player);
    gtk_box_pack_start(GTK_BOX(foo), timeline, TRUE, TRUE, 0);

    transport = transport_new(player);
    gtk_box_pack_start(GTK_BOX(foo),  transport->box, TRUE, TRUE, 0);

    gtk_widget_add_events(window, GDK_KEY_PRESS_MASK);
    g_signal_connect(window, "key_press_event", G_CALLBACK(keypress_handler), tree);
    g_signal_connect(window, "destroy", G_CALLBACK(destroy_handler), alphabet);

    /* g_timeout_add_full(G_PRIORITY_LOW, 1000, G_SOURCE_FUNC(update_timeline), timeline, NULL); */
    /* g_timeout_add_full(G_PRIORITY_LOW, 1000, G_SOURCE_FUNC(update_transport), transport, NULL); */
}

int window_run(int argc, char** argv)
{
    int status;
    GtkApplication* alphabet;

    player = player_init();
    if (!player) exit(EXIT_FAILURE);

    alphabet = gtk_application_new("org.gtk.alphabet", G_APPLICATION_HANDLES_OPEN);
    g_signal_connect(alphabet, "open", G_CALLBACK(on_open), NULL);
    g_signal_connect(alphabet, "activate", G_CALLBACK(on_activate), NULL);

    list = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);

    status = g_application_run(G_APPLICATION(alphabet), argc, argv);

    /* g_object_unref(alphabet); */
    g_application_quit(G_APPLICATION(alphabet));

    player_free(player);
    return status;
}
