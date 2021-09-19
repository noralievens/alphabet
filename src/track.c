/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @file        : track.c
 */

#include <assert.h>
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

static char* stralloc(const char* src);
static void set_libav_tags(Track* this);

static void set_r128(Track* this, SF_INFO* file_info, SNDFILE* file);
static void set_file_info(Track* this, SF_INFO* file_info);

Track* track_new(const char* name, const char* path)
{
    if (!path) return NULL;

    Track* this = malloc(sizeof(Track));
    SF_INFO file_info = { 0 };
    SNDFILE* file;

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

    set_libav_tags(this);

    if (!(file = sf_open(this->path, SFM_READ, &file_info))) {
        fprintf(stderr, "sndfile failed to open file\"%s\"\n", this->path);
        return NULL;
    }

    set_file_info(this, &file_info);
    set_r128(this, &file_info, file);

    if (sf_close(file) != 0) {
        fprintf(stderr, "sndfile failed to close file\"%s\"\n", this->path);
        return NULL;
    }

    this->duration = calloc(9, sizeof(char));
    dtoduration(this->duration, this->length);

    track_print(this);
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

    if (this->path) free(this->path);
    if (this->name) free(this->name);
    if (this->artist) free(this->artist);
    if (this->album) free(this->album);
    if (this->date) free(this->date);
    if (this->format) free(this->format);
    if (this->duration) free(this->duration);
    if (this->sample_rate) free(this->sample_rate);
    /* cannot - must not unref file ??*/
    free(this);
}


/*******************************************************************************
 * static functions
 *
 */

char* stralloc(const char* src)
{
    if (!src) return NULL;
    char* dest;
    if (!(dest = calloc(strlen(src)+1, sizeof(src)))) {
        fprintf(stderr, "could allocate string for \"%s\"\n", src);
    }
    strcpy(dest, src);

    return dest;
}



void set_libav_tags(Track* this)
{

    AVFormatContext* ctx = avformat_alloc_context();
    AVDictionaryEntry* tag = NULL;
    int flags = AV_DICT_IGNORE_SUFFIX;

    avformat_open_input(&ctx, this->path, NULL, NULL);
    avformat_find_stream_info(ctx, NULL);

    /* this->length = (double)ctx->duration / AV_TIME_BASE; */

    /* print all tags */
    /* while ((tag = av_dict_get(ctx->metadata, "", tag, flags))) {
        printf("TAG: %s = %s\n", tag->key, tag->value);
    } */

    if ((tag = av_dict_get(ctx->metadata, "title", NULL, flags))) {
        this->name = stralloc(tag->value);
    }
    if ((tag = av_dict_get(ctx->metadata, "name", NULL, flags))) {
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
    /* if ((tag = av_dict_get(ctx->metadata, "replaygain_track_gain", NULL, flags))) { */
    /*     double gain = strtod(tag->value, NULL); */
    /*     this->gain = gain; */
    /* } */
    /* if ((tag = av_dict_get(ctx->metadata, "replaygain_track_peak", NULL, flags))) { */
    /*     double peak = strtod(tag->value, NULL); */
    /*     this->peak = peak; */
    /* } */

    avformat_close_input(&ctx);
    avformat_free_context(ctx);
}

void set_file_info(Track* this, SF_INFO* file_info)
{
    this->sample_rate = calloc(11, sizeof(char));
    sprintf(this->sample_rate, "%d", file_info->samplerate);

    this->length = (double)file_info->frames / file_info->samplerate;
    return;
}

void set_r128(Track* this, SF_INFO* file_info, SNDFILE* file)
{
    sf_count_t frames_read;
    ebur128_state* sts = NULL;
    double* buffer;
    double lufs, peak;
    int flags = EBUR128_MODE_I | EBUR128_MODE_TRUE_PEAK;
    unsigned int sr = file_info->samplerate;
    unsigned int chs = file_info->channels;

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
