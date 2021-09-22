/**
 * @author      Arno Lievens (arnolievens@gmail.com)
 * @date        08/09/2021
 * @file        varispeed.h
 * @brief       varispeed widget
 * @copyright   Copyright (c) 2021 Arno Lievens
 */

#ifndef VARISPEED_H
#define VARISPEED_H

#include "../include/player.h"

typedef struct {
    Player* player;
    GtkWidget* box;
    GtkWidget* spin;
} Varispeed;

/**
 * Constructor
 *
 * create new varispeed counter
 *
 * @param player reference to the player object controlled
 * @return Varispeed object
 */
extern Varispeed* varispeed_new(Player* player);

/**
 * Free all resources
 *
 * @param this Varispeed object
 */
extern void varispeed_free(Varispeed* this);

#endif
