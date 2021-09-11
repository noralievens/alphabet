/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @created     : 11/09/2021
 * @filename    : transport.c
 */

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mpv/client.h>

#include "../include/player.h"

#include "../include/transport.h"

#define UNUSED __attribute__((unused))

static void on_clicked_forward(UNUSED GtkWidget *button, Player* player)
{
    player_seek(player, 1);
}

static void on_clicked_backward(UNUSED GtkWidget *button, Player* player)
{
    player_seek(player, -1);
}

static void on_clicked_play(UNUSED GtkWidget *button, Player* player)
{
    player_toggle(player);
}

static void on_clicked_pause(UNUSED GtkWidget *button, Player* player)
{
    player_toggle(player);
}

static void on_clicked_stop(UNUSED GtkWidget *button, Player* player)
{
    player_toggle(player);
    player_goto(player, 0);
}

GtkWidget* transport_new(Player* player)
{
    GtkWidget* box, * icon;

    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    icon = gtk_button_new_from_icon_name("media-seek-backward", GTK_ICON_SIZE_MENU);
    gtk_box_pack_start(GTK_BOX(box), icon, FALSE, FALSE, 0);
    g_signal_connect(icon, "clicked", G_CALLBACK(on_clicked_backward), player);

    icon = gtk_button_new_from_icon_name("media-playback-start", GTK_ICON_SIZE_MENU);
    gtk_box_pack_start(GTK_BOX(box), icon, FALSE, FALSE, 0);
    g_signal_connect(icon, "clicked", G_CALLBACK(on_clicked_play), player);

    icon = gtk_button_new_from_icon_name("media-playback-pause", GTK_ICON_SIZE_MENU);
    gtk_box_pack_start(GTK_BOX(box), icon, FALSE, FALSE, 0);
    g_signal_connect(icon, "clicked", G_CALLBACK(on_clicked_pause), player);

    icon = gtk_button_new_from_icon_name("media-playback-stop", GTK_ICON_SIZE_MENU);
    gtk_box_pack_start(GTK_BOX(box), icon, FALSE, FALSE, 0);
    g_signal_connect(icon, "clicked", G_CALLBACK(on_clicked_stop), player);

    icon = gtk_button_new_from_icon_name("media-seek-forward", GTK_ICON_SIZE_MENU);
    gtk_box_pack_start(GTK_BOX(box), icon, FALSE, FALSE, 0);
    g_signal_connect(icon, "clicked", G_CALLBACK(on_clicked_forward), player);

    return box;
}
