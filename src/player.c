/**
 * @author      Arno Lievens (arnolievens@gmail.com)
 * @date        08/09/2021
 * @file        player.c
 * @brief       mpv wrapper
 * @copyright   Copyright (c) 2021 Arno Lievens
 */

#include <assert.h>
#include <errno.h>
#include <locale.h>
#include <math.h>
#include <mpv/client.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/config.h"
#include "../include/track.h"

#include "../include/player.h"


static double db_to_volume(double volume);
static void mpv_print_status(const char* cmd, int status);


void player_set_speed(Player* this, gdouble speed)
{
    int status;
    if (speed < 0.1 || speed > 10.0) return;
    this->speed = speed;

    if ((status = mpv_set_property(this->mpv, "speed", MPV_FORMAT_DOUBLE, &this->speed)) < 0) {
        mpv_print_status("speed", status);
    }
}

void player_stop(Player* this)
{
    int status;
    const char* cmd[] = {"stop", NULL};

    player_goto(this, 0.0);

    /* update the play_state here to running load_track
     * whit outdated play_state due to async state update
     */

    this->play_state = PLAY_STATE_STOP;

    if ((status = mpv_command(this->mpv, cmd)) < 0) {
        mpv_print_status("stop", status);
    }

    if (this->current) {
        player_load_track(this, this->current);
    }

    player_pause(this);
}

void player_pause(Player* this)
{
    int status;
    int pause = 1;

    if ((status = mpv_set_property(this->mpv, "pause", MPV_FORMAT_FLAG, &pause)) < 0) {
        mpv_print_status("pause", status);
    }
    if (this->rtn) player_goto(this, 0);
}

void player_toggle(Player* this)
{
    int status;

    const char* cmd[] = {"cycle", "pause", NULL};
    if ((status = mpv_command(this->mpv, cmd)) < 0) {
        mpv_print_status("cycle, pause", status);
    }
    if (this->rtn) player_goto(this, 0);
}

void player_seek(Player* this, double secs)
{
    int status;
    char secstr[10];
    sprintf(secstr, "%f", secs);
    const char* cmd[] = {"seek", secstr, NULL};
    if ((status = mpv_command(this->mpv, cmd)) < 0) {
        mpv_print_status("seek", status);
    }
}

void player_loop(Player* this)
{
    int status;
    const char* cmd[] = {"ab-loop", NULL};

    /* cancel loop */
    if (this->loop_start != 0.0 && this->loop_stop != 0.0) {
        this->loop_start = 0.0;
        this->loop_stop = 0.0;

    /* mark loop stop (B) */
    } else if (this->loop_start != 0.0) {
        this->loop_stop = player_get_position(this);

    /* mark loop start (A) */
    } else {
        this->loop_stop = 0.0;
        this->loop_start = player_get_position(this);
    }

    if ((status = mpv_command(this->mpv, cmd)) < 0) {
        mpv_print_status("ab-loop", status);
    }
}

void player_mark(Player* this)
{
    this->marker = player_get_position(this);
}

void player_goto(Player* this, double position)
{
    if (!this->current) return;
    int status;
    position = CLAMP(position, 0, this->current->length);

    char posstr[9];
    g_snprintf(posstr, ELEMENTS(posstr), "%f", position);

    const char* cmd[] = {"seek", posstr, "absolute+keyframes", NULL};
    if ((status = mpv_command_async(this->mpv, 0, cmd)) < 0) {
        mpv_print_status("seek", status);
    }
}

/** deprecated */
double player_get_position(Player* this)
{
    mpv_get_property(this->mpv, "time-pos", MPV_FORMAT_DOUBLE, &this->position);
    return this->position;
}

void player_load_track(Player* this, Track* track)
{
    int status;
    char posstr[99];
    double gain, volume, position = 0.0;

    /* automatically deduce the position
     * when STOPPED position reverts to 0
     * when marker is set, position reverts to marker
     * when rtn is not set, position is the current playing position
     */

    if (this->play_state != PLAY_STATE_STOP) {

        if (this->marker != 0.0) {
            position = this->marker;

        } else if (!this->rtn) {
            position = player_get_position(this);
        }
    }

    gain = this->min_lufs - track->lufs;
    volume = db_to_volume(gain);

    /* compensation for time-gap? */
    /* if (position != 0.0) position += 0.050; */

    g_snprintf(posstr,
            ELEMENTS(posstr),
            "start=%d.%d,volume=%d.%d",
            (int)position,
            (int)(fmod(position, 1.0)*10000000),
            (int)volume,
            (int)(fmod(volume, 1.0)*10000000)
    );

    this->current = track;

    const char *cmd[] = {"loadfile", track->path, "replace", posstr, NULL};
    if ((status = mpv_command_async(this->mpv, 0, cmd)) < 0) {
        mpv_print_status("loadfile", status);
    }
}

int player_event_handler(Player* this)
{
	while (this->mpv) {

		mpv_event *event = mpv_wait_event(this->mpv, 0);

        switch (event->event_id) {
            case MPV_EVENT_PROPERTY_CHANGE: {
                mpv_event_property *prop = event->data;
                if (!prop->data) break;

                if (g_strcmp0(prop->name, "time-pos") == 0) {
                    this->position = *(double*)(prop->data);

                } else if (g_strcmp0(prop->name, "core-idle") == 0) {
                    int core_idle = *(int*)(prop->data);
                    this->play_state = core_idle ? PLAY_STATE_PAUSE : PLAY_STATE_PLAY;

                } else if (g_strcmp0(prop->name, "length") == 0) {
                    if (this->current) {
                        this->current->length = *(double*)(prop->data);
                    }
                }
                break;
            }
            case MPV_EVENT_LOG_MESSAGE: {
                printf("mpv log: %s", (char*)event->data);
                break;
            }
            case MPV_EVENT_SHUTDOWN:
            case MPV_EVENT_NONE: {
                return FALSE;
            }
            default: {
                break;
            };
        }
    }
    return FALSE;
}

void player_set_gain(Player* this, double gain)
{
    int status;

    if ((status = mpv_set_property(this->mpv, "replaygain-preamp", MPV_FORMAT_DOUBLE, &gain)) < 0) {
        mpv_print_status("replay-gain-preamp", status);
    }
}

void player_set_event_callback(Player* this, void(*event_callback)(void*))
{
	mpv_set_wakeup_callback(this->mpv, event_callback, this);
}

Player* player_init()
{
    int status;
    int false = 0;
    /* int true = 1; */

    Player* this = malloc(sizeof(Player));

    this->current = NULL;
    this->loop_start = 0;
    this->loop_stop = 0;
    this->marker = 0;
    this->play_state = PLAY_STATE_STOP;
    this->position = 0;
    this->rtn = 0;

    setlocale(LC_NUMERIC, "C");
    this->mpv = mpv_create();
    if (!this->mpv) {
        fprintf(stderr, "failed creating context\n");
        return NULL;
    }
	mpv_observe_property(this->mpv, 0, "core-idle", MPV_FORMAT_FLAG);
    mpv_observe_property(this->mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);
	mpv_observe_property(this->mpv, 0, "length", MPV_FORMAT_DOUBLE);

    if ((status = mpv_set_property(this->mpv, "audio-pitch-correction", MPV_FORMAT_FLAG, &false)) < 0) {
        mpv_print_status("audio-pitch-correction", status);
    }

    if ((status = mpv_set_property(this->mpv, "audio-display", MPV_FORMAT_FLAG, &false)) < 0) {
        mpv_print_status("audio-display", status);
    }

    if ((status = mpv_initialize(this->mpv)) < 0) {
        mpv_print_status("initialize", status);
    }
    return this;
}

void player_free(Player* this)
{
    if (!this) return;

    mpv_terminate_destroy(this->mpv);
    if (this->current) free(this->current);
    free(this);
}

double volume_to_db(double volume)
{
    return 60.0 * log(volume / 100.0) / log(10.0);
}

double db_to_volume(double db)
{
    return exp(log(10.0)*(db/60.0 + 2));
}

void mpv_print_status(const char* cmd, int status)
{
    fprintf(stderr, "mpv error for command: \"%s\"\n > %s\n",
            cmd, mpv_error_string(status));
}

