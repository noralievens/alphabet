/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @created     : 11/09/2021
 * @filename    : transport.h
 */

#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <gtk/gtk.h>

#include "../include/player.h"

typedef struct {
    Player* player;
    GtkWidget* box_movement;
    GtkWidget* box_control;
    GtkWidget* previous;
    GtkWidget* backward;
    GtkWidget* forward;
    GtkWidget* next;
    GtkWidget* play;
    GtkWidget* pause;
    GtkWidget* stop;
    GtkWidget* rtn;
    GtkWidget* ctd;
    GtkWidget* mark;
    GtkWidget* loop;
    GtkWidget* noloop;
} Transport;

/**
 * Constructor
 *
 * @param player object controlled by transport
 * @return this player object
 */
extern Transport* transport_new(Player* player);

/**
 * Update transport buttons
 *
 * call to update transport buttons state
 * actual state is fetched from player
 *
 * @param player object controlled by transport
 */
extern void transport_update(Transport* this);

/**
 * Free all resources
 *
 * gtk objects are destroyed
 * player remains untouched, should be destroyed by caller
 *
 * @param player object controlled by transport
 */
extern void transport_free(Transport* this);

#endif

// vim:ft=c

