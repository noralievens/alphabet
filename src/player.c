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
        printf("mpv API error: %s\n", mpv_error_string(status));
    }
}

void player_toggle(mpv_handle* this)
{
    const char* cmd[] = {"cycle", "pause", NULL};
    check_error(mpv_command(this, cmd));
}

void player_seek(mpv_handle* this, gdouble secs)
{
    char secstr[10];
    sprintf(secstr, "%f", secs);
    const char* cmd[] = {"seek", secstr, NULL};
    printf("seek: %s\n", secstr);
    check_error(mpv_command(this, cmd));
}

void player_loop(mpv_handle* this)
{
    const char* cmd[] = {"ab-loop", NULL};
    check_error(mpv_command(this, cmd));
}

void player_set_position(mpv_handle* this, gdouble position)
{
    printf("pos: %f\n", position);
    char posstr[10];
    sprintf(posstr, "%f", position);
    const char* cmd[] = {"seek", posstr, "absolute", NULL};
    check_error(mpv_command(this, cmd));
    /* check_error(mpv_set_property(this, "time-pos", MPV_FORMAT_DOUBLE, &position)); */
}

gdouble player_get_position(mpv_handle* this)
{
    gdouble position = 0.0;
    mpv_get_property(this, "time-pos", MPV_FORMAT_DOUBLE, &position);
    return position;
}

void player_load_track(mpv_handle* this, Track* track, gdouble position)
{
    char posstr[18];
    sprintf(posstr, "start=+%f", position);
    const char *cmd[] = {"loadfile", track->uri, "replace", posstr, NULL};
    check_error(mpv_command(this, cmd));
}

mpv_handle* player_init(void)
{
    mpv_handle *mpv = mpv_create();
    if (!mpv) {
        printf("failed creating context\n");
        return NULL;
    }

    int val = 1;
    check_error(mpv_set_option(mpv, "osc", MPV_FORMAT_FLAG, &val));

    // Done setting up options.
    check_error(mpv_initialize(mpv));
    return mpv;
}
