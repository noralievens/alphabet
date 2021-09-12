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

#define ICON_SIZE GTK_ICON_SIZE_MENU

#define UNUSED __attribute__((unused))


static void on_clicked_previous(UNUSED GtkWidget *button, UNUSED Transport* this)
{
    /* TODO */
}

static void on_clicked_forward(UNUSED GtkWidget *button, Transport* this)
{
    player_seek(this->player, 1);
}

static void on_clicked_backward(UNUSED GtkWidget *button, Transport* this)
{
    player_seek(this->player, -1);
}

static void on_clicked_next(UNUSED GtkWidget *button, UNUSED Transport* this)
{
    /* TODO */
}

static void on_clicked_play(UNUSED GtkWidget *button, Transport* this)
{
    player_toggle(this->player);
}

static void on_clicked_stop(UNUSED GtkWidget *button, Transport* this)
{
    player_toggle(this->player);
    player_goto(this->player, 0);
}

static void on_clicked_rtn(UNUSED GtkWidget *button, UNUSED Transport* this)
{
    this->player->rtn = !this->player->rtn;
    transport_update(this);
}

void transport_update(Transport* this)
{
    if (this->player->rtn) {
        gtk_widget_show(this->rtn);
        gtk_widget_hide(this->ctd);
    } else {
        gtk_widget_hide(this->rtn);
        gtk_widget_show(this->ctd);
    }

    if (this->player->play_state == PLAY_STATE_PLAY) {
        gtk_widget_show(this->pause);
        gtk_widget_hide(this->play);
    } else {
        gtk_widget_hide(this->pause);
        gtk_widget_show(this->play);
    }
}

Transport* transport_new(Player* player)
{
    GtkWidget* button;

    Transport* this = malloc(sizeof(Transport));
    this->player = player;

    this->box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    button = gtk_button_new_from_icon_name("media-skip-backward-symbolic", ICON_SIZE);
    gtk_box_pack_start(GTK_BOX(this->box), button, FALSE, FALSE, 0);
    g_signal_connect(button, "clicked", G_CALLBACK(on_clicked_previous), this);
    this->previous = button;

    button = gtk_button_new_from_icon_name("media-seek-backward-symbolic", ICON_SIZE);
    gtk_box_pack_start(GTK_BOX(this->box), button, FALSE, FALSE, 0);
    g_signal_connect(button, "clicked", G_CALLBACK(on_clicked_backward), this);
    this->backward = button;

    button = gtk_button_new_from_icon_name("media-seek-forward-symbolic", ICON_SIZE);
    gtk_box_pack_start(GTK_BOX(this->box), button, FALSE, FALSE, 0);
    g_signal_connect(button, "clicked", G_CALLBACK(on_clicked_forward), this);
    this->forward = button;

    button = gtk_button_new_from_icon_name("media-skip-forward-symbolic", ICON_SIZE);
    gtk_box_pack_start(GTK_BOX(this->box), button, FALSE, FALSE, 0);
    g_signal_connect(button, "clicked", G_CALLBACK(on_clicked_next), this);
    this->next = button;

    button = gtk_button_new_from_icon_name("media-playback-start-symbolic", ICON_SIZE);
    gtk_box_pack_start(GTK_BOX(this->box), button, FALSE, FALSE, 0);
    g_signal_connect(button, "clicked", G_CALLBACK(on_clicked_play), this);
    this->play = button;

    button = gtk_button_new_from_icon_name("media-playback-pause-symbolic", ICON_SIZE);
    gtk_box_pack_start(GTK_BOX(this->box), button, FALSE, FALSE, 0);
    g_signal_connect(button, "clicked", G_CALLBACK(on_clicked_play), this);
    this->pause = button;

    button = gtk_button_new_from_icon_name("media-playback-stop-symbolic", ICON_SIZE);
    gtk_box_pack_start(GTK_BOX(this->box), button, FALSE, FALSE, 0);
    g_signal_connect(button, "clicked", G_CALLBACK(on_clicked_stop), this);
    this->stop = button;

    button = gtk_button_new_from_icon_name("media-playlist-consecutive-symbolic-rtl", ICON_SIZE);
    gtk_box_pack_start(GTK_BOX(this->box), button, FALSE, FALSE, 0);
    g_signal_connect(button, "clicked", G_CALLBACK(on_clicked_rtn), this);
    this->rtn = button;

    button = gtk_button_new_from_icon_name("media-playlist-consecutive-symbolic", ICON_SIZE);
    gtk_box_pack_start(GTK_BOX(this->box), button, FALSE, FALSE, 0);
    g_signal_connect(button, "clicked", G_CALLBACK(on_clicked_rtn), this);
    this->ctd = button;

    gtk_widget_show_all(this->box);

    transport_update(this);

    return this;
}