/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @created     : 12/09/2021
 * @filename    : counter.c
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
#include <math.h>

#include "../include/track.h"
#include "../include/player.h"

#include "../include/counter.h"

void counter_update(Counter* this)
{
    if (!this || !this->player) return;
    /* if (this->player->position - this->position < 0.1) return; */
    /* this->position = this->player->position; */
    char count[100];
    gdouble seconds = this->player->position;
    seconds *= seconds < 0 ? -1 : 1;
    sprintf(count, "<big><tt>%d:%.2d.%.3d</tt></big>", (int)(seconds/60), (int)(fmod(seconds,60)), (int)(fmod(seconds,1)*1000));
    gtk_label_set_markup(GTK_LABEL(this->label), count);
}

Counter* counter_new(Player* player)
{
    Counter* this = malloc(sizeof(Counter));

    this->player = player;
    this->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    this->position = 0;

    this->label = gtk_label_new("0:00.00");
    gtk_box_pack_start(GTK_BOX(this->box), this->label, FALSE, FALSE, 0);

    gtk_widget_show_all(this->box);

    return this;
}

void counter_free(Counter* this)
{
    if (!this) return;
    gtk_widget_destroy(this->box);
    free(this);
}
