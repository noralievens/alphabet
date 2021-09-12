/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @created     : 11/09/2021
 * @filename    : transport.h
 */

#ifndef TRANSPORT_H
#define TRANSPORT_H

#include "../include/player.h"

typedef struct {
    Player* player;
    GtkWidget* box;
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

extern Transport* transport_new(Player* player);

extern void transport_update(Transport* this);

#endif

// vim:ft=c

