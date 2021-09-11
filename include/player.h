/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @created     : 08/09/2021
 * @filename    : player.h
 */

#ifndef PLAYER_H
#define PLAYER_H

#include "../include/track.h"

#include <mpv/client.h>

typedef enum {
    PLAY_STATE_PLAY,
    PLAY_STATE_STOP,
    PLAY_STATE_PAUSE,
} PlayState;

typedef struct {
    mpv_handle* mpv;
    Track* current;
    gdouble position;
    gdouble loop_start;
    gdouble loop_stop;
    gdouble marker;
    PlayState play_state;
} Player;

extern void player_toggle(Player* this);

extern void player_seek(Player* this, gdouble secs);

extern void player_loop(Player* this);

extern void player_mark(Player* this);

extern void player_goto(Player* this, gdouble position);

extern gdouble player_update(Player* this);

extern void player_load_track(Player* this, Track* track, gdouble position);

extern Player* player_init(void);

extern void player_free(Player* this);

#endif

// vim:ft=c

