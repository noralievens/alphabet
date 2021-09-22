/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @date        : 08/09/2021
 * @file        : counter.h
 * @brief       : time counter widget
 * @copyright   : Copyright (c) 2021 Arno Lievens
 */

#ifndef COUNTER_H
#define COUNTER_H

#include <gtk/gtk.h>

#include "../include/player.h"

typedef struct {
    GtkWidget* box;
    GtkWidget* label;
    Player* player;
    gdouble position;
} Counter;

/**
 * Constructor
 *
 * create new counter widget
 *
 * @param player object represented by counter
 * @return timeline object
 */
extern Counter* counter_new(Player* player);

/**
* update timeline
*
* call to notify timeline should update itself
* @param this the timeline object
*/
extern void counter_update(Counter* this);

/**
 * Free all resources
 *
* @param this the counter object
 */
extern void counter_free(Counter* this);

#endif

/* vim:ft=c */
