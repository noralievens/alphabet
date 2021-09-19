/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @file        : player.c
 */

#include <assert.h>
#include <errno.h>
#include <mpv/client.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "../include/config.h"
#include "../include/track.h"

#include "../include/player.h"

#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef CLAMP
#define CLAMP(x, low, high) (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#endif

#ifndef ELEMENTS
#define ELEMENTS(arr) (sizeof (arr) / sizeof ((arr)[0]))
#endif


/* static double volume_to_db(double volume); */
static double db_to_volume(double volume);
static void mpv_print_status(int status);

void player_set_speed(Player* this, gdouble speed)
{
    int status;
    if (speed < 0.1 || speed > 10.0) return;
    this->speed = speed;

    if ((status = mpv_set_property(this->mpv, "speed", MPV_FORMAT_DOUBLE, &this->speed)) < 0) {
        mpv_print_status(status);
    }
}

void player_stop(Player* this)
{
    int status;
    const char* cmd[] = {"stop", NULL};
    if ((status = mpv_command(this->mpv, cmd)) < 0) {
        mpv_print_status(status);
    }
    player_load_track(this, this->current, 0);
    player_pause(this);
    if (this->rtn) player_goto(this, 0);
}

void player_pause(Player* this)
{
    int status;
    int pause = 1;

    if ((status = mpv_set_property(this->mpv, "pause", MPV_FORMAT_FLAG, &pause)) < 0) {
        mpv_print_status(status);
    }
    if (this->rtn) player_goto(this, 0);
}

void player_toggle(Player* this)
{
    /* FIXME find proper play - pause methods */
    int status;

    const char* cmd[] = {"cycle", "pause", NULL};
    if ((status = mpv_command(this->mpv, cmd)) < 0) {
        mpv_print_status(status);
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
        mpv_print_status(status);
    }
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

void player_goto(Player* this, double position)
{
    int status;
    if (position > this->current->length) position = this->current->length;
    if (position < 0) position = 0;

    char posstr[9];
    g_snprintf(posstr, ELEMENTS(posstr), "%f", position);

    const char* cmd[] = {"seek", posstr, "absolute+keyframes", NULL};
    if ((status = mpv_command_async(this->mpv, 0, cmd)) < 0) {
        mpv_print_status(status);
    }
}

/** deprecated */
double player_update(Player* this)
{
    mpv_get_property(this->mpv, "time-pos", MPV_FORMAT_DOUBLE, &this->position);
    return this->position;
}

void player_load_track(Player* this, Track* track, double position)
{
    int status;
    char posstr[99];
    position = CLAMP(position, 0, track->length);
    double gain, volume;


    gain = this->min_lufs - track->lufs;
    volume = db_to_volume(gain);

    /* compensation for time-gap? */
    if (position) position += 0.050;

    g_snprintf(posstr,
            ELEMENTS(posstr),
            "start=%d.%d,volume=%f",
            (int)position,
            (int)(fmod(position, 1.0)*10000000),
            volume
    );

    this->current = track;

    const char *cmd[] = {"loadfile", track->path, "replace", posstr, NULL};
    if ((status = mpv_command_async(this->mpv, 0, cmd)) < 0) {
        mpv_print_status(status);
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
                    this->current->length = *(double*)(prop->data);
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
        mpv_print_status(status);
    }
}

void player_set_event_callback(Player* this, void(*event_callback)(void*))
{
	mpv_set_wakeup_callback(this->mpv, event_callback, this);
}

Player* player_init()
{
    int status;
    int pitch_correction = 0;
    /* const char* replay_gain = "track"; */

    Player* this = malloc(sizeof(Player));

    this->current = NULL;
    this->loop_start = 0;
    this->loop_stop = 0;
    this->marker = 0;
    this->play_state = PLAY_STATE_STOP;
    this->position = 0;
    this->rtn = 0;

    this->mpv = mpv_create();
    if (!this->mpv) {
        fprintf(stderr, "failed creating context\n");
        return NULL;
    }
	mpv_observe_property(this->mpv, 0, "core-idle", MPV_FORMAT_FLAG);
    mpv_observe_property(this->mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);
	mpv_observe_property(this->mpv, 0, "length", MPV_FORMAT_DOUBLE);

    if ((status = mpv_set_property(this->mpv, "audio-pitch-correction", MPV_FORMAT_FLAG, &pitch_correction)) < 0) {
        mpv_print_status(status);
    }

    /* if ((status = mpv_set_property(this->mpv, "replaygain", MPV_FORMAT_STRING, &replay_gain)) < 0) { */
    /*     mpv_print_status(status); */
    /* } */

    if ((status = mpv_initialize(this->mpv)) < 0) {
        mpv_print_status(status);
    }
    return this;
}

void player_free(Player* this)
{
    if (!this) return;
    mpv_terminate_destroy(this->mpv);
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

void mpv_print_status(int status)
{
    fprintf(stderr, "mpv error: %s\n", mpv_error_string(status));
}

