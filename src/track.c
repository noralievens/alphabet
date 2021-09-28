/**
 * @author      Arno Lievens (arnolievens@gmail.com)
 * @date        08/09/2021
 * @file        track.c
 * @brief       track class
 * @copyright   Copyright (c) 2021 Arno Lievens
 */

#include <ebur128.h>
#include <errno.h>
#include <libavformat/avformat.h>
#include <libavutil/dict.h>
#include <math.h>
#include <sndfile.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/config.h"

#include "../include/track.h"

/**
 * Set metadata if available
 *
 * overrides the .name property if TITLE or NAME is available
 *
 * @param track the Track object
 */
static void track_set_libav_tags(Track* this);

/**
 * Calculate aver loudness
 *
 * sets the loudness .lufs property in track
 *
 * @param this the track object
 */
static void track_set_r128(Track* this, SF_INFO* file_info, SNDFILE* file);

/**
 * Read file info with libsndfile
 *
 * sets the samplerate, format, ... properties in track
 *
 * @param this the track object
 * @param file_info the file info object
 */
static void track_set_file_info(Track* this, SF_INFO* file_info);

/**
 * Allocate and copy string
 *
 * convenience function to easily malloc and copy string
 * must be free-ed by caller
 *
 * @param src the string to be copied
 * @return the new string or NULL when failed to allocate
 */
static char* stralloc(const char* src);


/*******************************************************************************
 * extern functions
 */


Track* track_new(const char* name, const char* path)
{
    Track* this = NULL;
    SF_INFO file_info = { 0 };
    SNDFILE* file;

    /* allocate new track and set defaults
     * the name used by default is probided by the argument but overwritten
     * by TITLE or NAME tag if present
     * average loudness is calculated by r123
     */

    if (!path) goto fail;

    if (!(this = malloc(sizeof(Track)))) {
        fprintf(stderr, "failed to allocate track\n");
        goto fail;
    }

    this->artist = NULL;
    this->album = NULL;
    this->date = NULL;
    this->offset = 0;
    this->lufs = 0;
    this->peak = 0;
    this->format = 0;
    this->length = 0;
    this->sample_rate = NULL;

    this->path = stralloc(path);
    if (name) this->name = stralloc(name);
    else this->name = stralloc(path);

    track_set_libav_tags(this);

    if (!(file = sf_open(this->path, SFM_READ, &file_info))) {
        fprintf(stderr, "sndfile failed to open file\"%s\"\n", this->path);
        return NULL;
    }

    track_set_file_info(this, &file_info);
    track_set_r128(this, &file_info, file);

    if (sf_close(file) != 0) {
        fprintf(stderr, "sndfile failed to close file\"%s\"\n", this->path);
        return NULL;
    }

    this->duration = calloc(9, sizeof(char));
    dtoduration(this->duration, this->length);

fail:
    return this;
}

void track_print(Track* this)
{
    printf("path       = %s\n", this->path);
    printf("name       = %s\n", this->name);
    printf("artist     = %s\n", this->artist);
    printf("album      = %s\n", this->album);
    printf("date       = %s\n", this->date);
    printf("duration   = %s\n", this->duration);
    printf("format     = %s\n", this->format);
    printf("samplerate = %s\n", this->sample_rate);
    printf("lufs       = %f\n", this->lufs);
    printf("peak       = %f\n", this->peak);
    printf("\n");
}

void track_free(Track* this)
{
    if (!this) return;

    free(this->path);
    free(this->name);
    free(this->artist);
    free(this->album);
    free(this->date);
    free(this->format);
    free(this->duration);
    free(this->sample_rate);
    free(this);
}


/*******************************************************************************
 * static functions
 *
 */


char* stralloc(const char* src)
{
    char* dest;

    if (!src) return NULL;
    if (!(dest = calloc(strlen(src)+1, sizeof(src)))) {
        fprintf(stderr, "could allocate string for \"%s\"\n", src);
        return NULL;
    }
    strcpy(dest, src);

    return dest;
}

void track_set_libav_tags(Track* this)
{
    AVFormatContext* ctx = avformat_alloc_context();
    AVDictionaryEntry* tag = NULL;
    int flags = AV_DICT_IGNORE_SUFFIX;

    avformat_open_input(&ctx, this->path, NULL, NULL);
    avformat_find_stream_info(ctx, NULL);

    /* print all tags */
    /* while ((tag = av_dict_get(ctx->metadata, "", tag, flags))) {
        printf("TAG: %s = %s\n", tag->key, tag->value);
    } */

    if ((tag = av_dict_get(ctx->metadata, "title", NULL, flags))) {
        free(this->name);
        this->name = stralloc(tag->value);
    }
    if ((tag = av_dict_get(ctx->metadata, "name", NULL, flags))) {
        free(this->name);
        this->name = stralloc(tag->value);
    }
    if ((tag = av_dict_get(ctx->metadata, "artist", NULL, flags))) {
        this->artist = stralloc(tag->value);
    }
    if ((tag = av_dict_get(ctx->metadata, "album", NULL, flags))) {
        this->album = stralloc(tag->value);
    }
    if ((tag = av_dict_get(ctx->metadata, "date", NULL, flags))) {
        this->date = stralloc(tag->value);
    }

    avformat_close_input(&ctx);
    avformat_free_context(ctx);
}

void track_set_file_info(Track* this, SF_INFO* file_info)
{
    const size_t len = 7;
    this->sample_rate = calloc(len+1, sizeof(char));
    snprintf(this->sample_rate, len, "%d", file_info->samplerate);

    this->length = (double)file_info->frames / file_info->samplerate;
    return;
}

void track_set_r128(Track* this, SF_INFO* file_info, SNDFILE* file)
{
    sf_count_t frames_read;
    ebur128_state* sts = NULL;
    double* buffer;
    double lufs, peak;
    int flags = EBUR128_MODE_I | EBUR128_MODE_TRUE_PEAK;
    unsigned int sr = (unsigned int)file_info->samplerate;
    unsigned int chs = (unsigned int)file_info->channels;

    if (!(sts = malloc(sizeof(ebur128_state*)))) {
        fprintf(stderr, "ebur128 malloc failed\n");
        return;
    }

    if (!(sts = ebur128_init(chs, sr, flags))) {
        fprintf(stderr, "ebur128 could not create ebur128_state!\n");
        return;
    }

    if (!(buffer = malloc(sts->samplerate * sts->channels * sizeof(double)))) {
        fprintf(stderr, "ebur128 malloc failed\n");
        return;
    }

    while ((frames_read = sf_readf_double(file, buffer, (sf_count_t)sts->samplerate))) {
        ebur128_add_frames_double(sts, buffer, (size_t)frames_read);
    }

    ebur128_loudness_global(sts, &lufs);
    this->lufs = lufs;

    ebur128_sample_peak(sts, 0, &peak);
    this->peak = peak;

    free(buffer);
    buffer = NULL;

    ebur128_destroy(&sts);
    free(sts);
}
