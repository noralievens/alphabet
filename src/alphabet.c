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

#ifdef MAC_INTEGRATION
#include <gtkosxapplication.h>
#endif

#include "../include/config.h"
#include "../include/track.h"
#include "../include/player.h"
#include "../include/timeline.h"
#include "../include/transport.h"
#include "../include/varispeed.h"
#include "../include/counter.h"

GtkListStore* list;

Player* player;
Transport* transport;
Timeline* timeline;
Counter* counter;
Varispeed* varispeed;

static void on_activate(GtkApplication* alphabet);

enum {
    TREE_COLUMN_NAME,
    TREE_COLUMN_DURATION,
    TREE_COLUMN_DATA,
    TREE_COLUMNS
};

void on_selection_changed(GtkTreeSelection* selection, GtkTreeModel* model)
{
    Track* track;
    GtkTreeIter iter;
    gtk_tree_selection_get_selected(selection, &model, &iter);
    gtk_tree_model_get(model, &iter, TREE_COLUMN_DATA, &track, -1);

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
    gtk_tree_model_get(model, &iter, TREE_COLUMN_DATA, &track, -1);
    gtk_list_store_remove(list, &iter);
    track_free(track);
}

void add_track(GFile* file)
{
    /* TODO validate audio file ! */
    Track* track = track_new(g_file_get_basename(file), g_file_get_path(file));
    GtkTreeIter iter;
    gtk_list_store_append(list, &iter);
    gtk_list_store_set(list, &iter,
            TREE_COLUMN_NAME, track->name,
            TREE_COLUMN_DURATION, track->duration,
            TREE_COLUMN_DATA, track,
            -1);
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
            gtk_button_clicked(GTK_BUTTON(transport->rtn));
            return TRUE;

        case GDK_KEY_l:
            gtk_button_clicked(GTK_BUTTON(transport->loop));
            return TRUE;

        case GDK_KEY_space:
            gtk_button_clicked(GTK_BUTTON(transport->play));
            return TRUE;

        case GDK_KEY_Left:
            gtk_button_clicked(GTK_BUTTON(transport->backward));
            return TRUE;

        case GDK_KEY_Right:
            gtk_button_clicked(GTK_BUTTON(transport->forward));
            return TRUE;

        case GDK_KEY_m:
        case GDK_KEY_KP_Enter:
            player_mark(player);
            return TRUE;

        case GDK_KEY_Return:
            gtk_button_clicked(GTK_BUTTON(transport->stop));
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
    gtk_tree_view_column_set_title(column, "Track");
    gtk_tree_view_column_set_expand(column, TRUE);
    g_object_set(G_OBJECT(cellrenderer), "ellipsize", PANGO_ELLIPSIZE_END, NULL);
    gtk_tree_view_column_set_sort_column_id(column, id);

    id = 1;
    column = gtk_tree_view_column_new();
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);
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

    return tree;
}


/**
 * update the UI elements
 * no data is passed, just a trigger to update
 */
static gboolean update_ui(UNUSED gpointer data)
{
    timeline_update(timeline);
    counter_update(counter);
    transport_update(transport);
    return FALSE;
}


/**
 * event callback
 *
 * connect the player callback method to the player event
 *
 * called functions must be short and non-blocking
 */
static void event_callback(gpointer data)
{
    g_idle_add(G_SOURCE_FUNC(player_event_handler), data);
    g_idle_add(G_SOURCE_FUNC(update_ui), data);
}

void on_activate(GtkApplication* alphabet)
{
    GtkWidget* window, * box, *scrolled;
    GtkWidget* tree, * bar, * button;

#ifdef MAC_INTEGRATION
    GtkosxApplication* osx = g_object_new(GTKOSX_TYPE_APPLICATION, NULL);
#endif


    window = gtk_application_window_new(alphabet);
    gtk_window_set_title(GTK_WINDOW(window), "Alphabet");
    gtk_container_set_border_width(GTK_CONTAINER(window), 0);
    gtk_window_set_default_size(GTK_WINDOW(window), 640, 320);
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

    bar = gtk_action_bar_new();
    gtk_box_pack_end(GTK_BOX(box), bar, FALSE, FALSE, 0);

    gtk_widget_show_all(box);

    button = gtk_button_new_with_mnemonic("_Add");
    gtk_action_bar_pack_start(GTK_ACTION_BAR(bar), button);
    g_signal_connect(button, "clicked", G_CALLBACK(on_clicked_add), window);
    gtk_widget_show_all(button);

    counter = counter_new(player);
    /* gtk_box_pack_start(GTK_BOX(foo), counter->box, FALSE, FALSE, spacing*2); */
    gtk_action_bar_pack_start(GTK_ACTION_BAR(bar), counter->box);
    /* gtk_widget_set_margin_start(counter->box, MARGIN/2); */

    timeline = timeline_new(player);
    /* gtk_box_pack_start(GTK_BOX(foo), timeline->box, TRUE, TRUE, spacing*2); */
    gtk_action_bar_pack_start(GTK_ACTION_BAR(bar), timeline->box);

    varispeed = varispeed_new(player);
    /* gtk_box_pack_start(GTK_BOX(foo), varispeed->box, FALSE, FALSE, spacing*2); */
    gtk_action_bar_pack_end(GTK_ACTION_BAR(bar), varispeed->box);
    /* gtk_widget_set_margin_start(varispeed->box, MARGIN/2); */
    /* gtk_widget_set_margin_end(varispeed->box, MARGIN/2); */

    transport = transport_new(player);
    /* gtk_box_pack_end(GTK_BOX(foo),  transport->box, FALSE, FALSE, spacing*2); */
    gtk_action_bar_pack_end(GTK_ACTION_BAR(bar), transport->box_control);
    gtk_action_bar_pack_end(GTK_ACTION_BAR(bar), transport->box_movement);

    gtk_widget_add_events(window, GDK_KEY_PRESS_MASK);
    g_signal_connect(window, "key_press_event", G_CALLBACK(keypress_handler), tree);
    g_signal_connect(window, "destroy", G_CALLBACK(destroy_handler), alphabet);

    /* event_callback will be called whenever player received event */
    player_set_event_callback(player, event_callback);
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

    list = gtk_list_store_new(TREE_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);

    status = g_application_run(G_APPLICATION(alphabet), argc, argv);

    /* g_object_unref(alphabet); */
    g_application_quit(G_APPLICATION(alphabet));

    player_free(player);
    return status;
}

int main(int argc, char *argv[])
{
    return window_run(argc - optind, argv);
}
