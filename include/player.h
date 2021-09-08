/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @created     : 08/09/2021
 * @filename    : player.h
 */

#ifndef PLAYER_H
#define PLAYER_H

#include "../include/track.h"

#include <mpv/client.h>


extern void player_set_position(mpv_handle* this, gdouble position);

extern gdouble player_get_position(mpv_handle* this);

extern void player_load_track(mpv_handle* this, Track* track, gdouble position);

extern mpv_handle* player_init(void);

#endif

// vim:ft=c

