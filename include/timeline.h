/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @created     : 11/09/2021
 * @filename    : timeline.h
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
} Timeline;

extern void timeline_update(Timeline* this);

extern Timeline* timeline_new(Player* player);

#endif

// vim:ft=c

