/**
 * @author      Arno Lievens (arnolievens@gmail.com)
 * @date        08/09/2021
 * @file        transport.h
 * @brief       transport buttons widget
 * @copyright   Copyright (c) 2021 Arno Lievens
 */

#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <gtk/gtk.h>

#include "../include/player.h"

/**
 * Transport button widget
 *
 * allow the user to control the player by clicking buttons
 * current state of the player is displayed by the buttons
 */
typedef struct {
    Player* player;             /**< object controlled by transport */
    GtkWidget* box_movement;    /**< top-level for play/pause/stop ... */
    GtkWidget* box_control;     /**< top-level for rtn/loop/... */
    GtkWidget* previous;        /**< previous track button */
    GtkWidget* backward;        /**< move backward button */
    GtkWidget* forward;         /**< move forward button */
    GtkWidget* next;            /**< next track button */
    GtkWidget* play;            /**< play button - will swap with pause */
    GtkWidget* pause;           /**< pause button - will swap with play */
    GtkWidget* stop;            /**< stop button (return-to-zero) */
    GtkWidget* rtn;             /**< player will rtz on pause/play/selection */
    GtkWidget* ctd;             /**< player will continue on play/selection */
    GtkWidget* mark;            /**< set marker button/display */
    GtkWidget* loop;            /**< set loop A/B/cancel */
    GtkWidget* noloop;          /**< loop is canceled display */
} Transport;

/**
 * Constructor
 *
 * all transport widgets will "widget_show_all" themselves
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
