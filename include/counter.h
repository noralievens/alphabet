/**
 * @author      Arno Lievens (arnolievens@gmail.com)
 * @date        08/09/2021
 * @file        counter.h
 * @brief       time counter widget
 * @copyright   Copyright (c) 2021 Arno Lievens
 */

#ifndef COUNTER_H
#define COUNTER_H

#include <gtk/gtk.h>

#include "../include/player.h"

/**
 * Counter widget
 *
 * displays the current time position in player
 */
typedef struct {
    GtkWidget* box;             /**< top-level widget */
    GtkWidget* label;           /**< the actual counter text */
    Player* player;             /**< the object for whom the counter counts */
    gdouble position;           /**< the last updated time posion */
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
* new position will be queried from player
*
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
