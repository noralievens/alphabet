/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @date        : 08/09/2021
 * @file        : timeline.h
 * @brief       : timeline widget
 * @copyright   : Copyright (c) 2021 Arno Lievens
 */

#ifndef TIMELINE_H
#define TIMELINE_H

#include <gtk/gtk.h>

#include "../include/player.h"

typedef struct {
    GtkWidget* box;
    Player* player;
    GdkRGBA position;
    GdkRGBA loop;
    GdkRGBA marker;
    GdkRGBA wave;
    GtkImage* image;
} Timeline;

/**
 * Constructor
 *
 * create new timeline widget
 *
 * @param player object represented and controlled by timeline
 * @return timeline object
 */
extern Timeline* timeline_new(Player* player);

/**
* update timeline
*
* call to notify timeline should update itself
* @param this the timeline object
*/
extern void timeline_update(Timeline* this);

/**
 * Free all resources
 *
* @param this the timeline object
 */
extern void timeline_free(Timeline* this);

#endif
