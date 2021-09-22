/**
 * @author      Arno Lievens (arnolievens@gmail.com)
 * @date        08/09/2021
 * @file        counter.c
 * @brief       time counter widget
 * @copyright   Copyright (c) 2021 Arno Lievens
 */

#include <assert.h>
#include <errno.h>
#include <gtk/gtk.h>
#include <math.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/track.h"
#include "../include/player.h"

#include "../include/counter.h"

void counter_update(Counter* this)
{
    if (!this || !this->player) return;
    char count[100];
    gdouble seconds = this->player->position;
    seconds *= seconds < 0 ? -1 : 1;
    sprintf(count, "<big><tt>%02d:%02d.%03d</tt></big>", (int)(seconds/60), (int)(fmod(seconds,60)), (int)(fmod(seconds,1)*1000));
    gtk_label_set_markup(GTK_LABEL(this->label), count);
    /* FIXME: setting label emits size-allocate !!! check by connect signal and start playback*/
}

Counter* counter_new(Player* player)
{
    GtkWidget* frame;
    Counter* this = malloc(sizeof(Counter));

    this->player = player;
    this->box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    this->position = 0;

    frame = gtk_frame_new(NULL);
    gtk_box_pack_start(GTK_BOX(this->box), frame, TRUE, TRUE, 0);

    this->label = gtk_label_new("0:00.00");
    gtk_container_add(GTK_CONTAINER(frame), this->label);
    gtk_label_set_max_width_chars(GTK_LABEL(this->label), 9);
    gtk_widget_set_margin_start(this->label, MARGIN/2);
    gtk_widget_set_margin_end(this->label, MARGIN/2);
    gtk_widget_show_all(this->box);

    return this;
}

void counter_free(Counter* this)
{
    if (!this) return;
    gtk_widget_destroy(this->box);
    free(this);
}
