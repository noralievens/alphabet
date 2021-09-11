/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @created     : 08/09/2021
 * @filename    : player.c
 */

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mpv/client.h>

#include "../include/track.h"

#include "../include/player.h"

static inline void check_error(int status)
{
    if (status < 0) {
        fprintf(stderr, "mpv error: %s\n", mpv_error_string(status));
    }
}

static void mpv_print_status(int status)
{
    fprintf(stderr, "mpv error: %s\n", mpv_error_string(status));
}

void player_toggle(Player* this)
{
    const char* cmd[] = {"cycle", "pause", NULL};
    check_error(mpv_command(this->mpv, cmd));
}

void player_seek(Player* this, gdouble secs)
{
    char secstr[10];
    sprintf(secstr, "%f", secs);
    const char* cmd[] = {"seek", secstr, NULL};
    check_error(mpv_command(this->mpv, cmd));
}

void player_loop(Player* this)
{
    int status;
    const char* cmd[] = {"ab-loop", NULL};

    /* cancel loop */
    if (this->loop_start && this->loop_stop) {
        this->loop_start = 0.0;
        this->loop_stop = 0.0;

    /* mark loop stop (B) */
    } else if (this->loop_start) {
        this->loop_stop = player_update(this);

    /* mark loop start (A) */
    } else {
        this->loop_stop = 0.0;
        this->loop_start = player_update(this);
    }

    if ((status = mpv_command(this->mpv, cmd)) < 0) {
        mpv_print_status(status);
    }
}

void player_mark(Player* this)
{
    this->marker = player_update(this);
}

void player_goto(Player* this, gdouble position)
{
    char posstr[10];

    if (position > this->current->length) position = this->current->length;

    sprintf(posstr, "%f", position);
    const char* cmd[] = {"seek", posstr, "absolute", NULL};
    check_error(mpv_command(this->mpv, cmd));
}

gdouble player_update(Player* this)
{
    mpv_get_property(this->mpv, "time-pos", MPV_FORMAT_DOUBLE, &this->position);
    return this->position;
}

void player_load_track(Player* this, Track* track, gdouble position)
{
    char posstr[18];
    sprintf(posstr, "start=+%f", position);
    const char *cmd[] = {"loadfile", track->uri, "replace", posstr, NULL};
    check_error(mpv_command(this->mpv, cmd));
    this->current = track;
}

Player* player_init(void)
{
    Player* this = calloc(1, sizeof(Player));

    this->mpv = mpv_create();
    if (!this->mpv) {
        fprintf(stderr, "failed creating context\n");
        return NULL;
    }
    check_error(mpv_initialize(this->mpv));
    return this;
}

void player_free(Player* this)
{
    if (!this) return;
    mpv_terminate_destroy(this->mpv);
    free(this);
}
