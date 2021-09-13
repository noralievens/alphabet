/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @created     : 13/09/2021
 * @filename    : varispeed.c
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

#include "../include/varispeed.h"

#define UNUSED __attribute__((unused))

static void on_value_changed(Varispeed* this, GtkSpinButton *spin_button)
{
    gdouble value = gtk_spin_button_get_value(spin_button);
    player_set_speed(this->player, value);
}

void varispeed_update(Varispeed* this)
{
}

Varispeed* varispeed_new(Player* player)
{
    Varispeed* this = malloc(sizeof(Varispeed));

    this->player = player;

    this->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    /* this->switch = gtk_switch_new( */

    this->spin = gtk_spin_button_new_with_range(0.1, 10.0, 0.001);
    gtk_box_pack_start(GTK_BOX(this->box), this->spin, FALSE, FALSE, 0);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(this->spin), 1.0);
    g_signal_connect_swapped(this->spin, "value-changed", G_CALLBACK(on_value_changed), this);

    gtk_widget_show_all(this->box);


    return this;
}
