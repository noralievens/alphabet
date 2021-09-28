/**
 * @author      Arno Lievens (arnolievens@gmail.com)
 * @date        08/09/2021
 * @file        player.h
 * @brief       mpv wrapper
 * @copyright   Copyright (c) 2021 Arno Lievens
 */

#ifndef PLAYER_H
#define PLAYER_H

#include "../include/track.h"

#include <mpv/client.h>

typedef enum PlayState {
    PLAY_STATE_PLAY,
    PLAY_STATE_STOP,
    PLAY_STATE_PAUSE,
} PlayState;

typedef struct Player {
    mpv_handle* mpv;
    Track* current;
    double position;
    double loop_start;
    double loop_stop;
    double marker;
    PlayState play_state;
    int rtn;
    double speed;
    double min_lufs;
} Player;

extern void player_set_gain(Player* this, double gain);

extern void player_set_speed(Player* this, double speed);

extern void player_stop(Player* this);

extern void player_pause(Player* this);

extern void player_toggle(Player* this);

extern void player_seek(Player* this, double secs);

extern void player_loop(Player* this);

extern void player_mark(Player* this);

extern void player_goto(Player* this, double position);

extern double player_get_position(Player* this);

extern void player_load_track(Player* this, Track* track);

extern int player_event_handler(Player* this);

extern void player_set_event_callback(Player* this, void(*event_callback)(void*));

extern Player* player_init(void);

extern void player_free(Player* this);

#endif
