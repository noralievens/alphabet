/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @file        : counter.h
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


extern void counter_update(Counter* this);

extern Counter* counter_new(Player* player);

extern void counter_free(Counter* this);

#endif

// vim:ft=c

