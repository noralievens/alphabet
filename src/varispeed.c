/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @date        : 08/09/2021
 * @file        : varispeed.c
 * @brief       : varispeed widget
 * @copyright   : Copyright (c) 2021 Arno Lievens
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
#include "../include/config.h"

#include "../include/varispeed.h"

#define UNUSED __attribute__((unused))

static void on_value_changed(Varispeed* this, GtkSpinButton *spin_button)
{
    gdouble value = gtk_spin_button_get_value(spin_button);
    player_set_speed(this->player, value);
}

Varispeed* varispeed_new(Player* player)
{
    Varispeed* this = malloc(sizeof(Varispeed));

    this->player = player;

    this->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, MARGIN);

    /* this->switch = gtk_switch_new( */

    this->spin = gtk_spin_button_new_with_range(0.1, 10.0, 0.001);
    gtk_box_pack_start(GTK_BOX(this->box), this->spin, FALSE, FALSE, 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(this->spin), 1.0);
    g_signal_connect_swapped(this->spin, "value-changed", G_CALLBACK(on_value_changed), this);

    gtk_widget_show_all(this->box);
    return this;
}

void varispeed_update(UNUSED Varispeed* this)
{
}

void varispeed_free(Varispeed* this)
{
    if (!this) return;

    g_signal_handlers_disconnect_by_data(this->spin, this);
    gtk_widget_destroy(this->box);
}
