/**
 * @author      : Arno Lievens (arnolievens@gmail.com)
 * @date        : 08/09/2021
 * @file        : config.c
 * @brief       : config veriables and common helper functions
 * @copyright   : Copyright (c) 2021 Arno Lievens
 */

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "../include/config.h"

void dtoduration(char* dest, double num)
{
    unsigned int min, sec, msec;

    min = (unsigned int)num / 60;
    sec = (unsigned int)num % 60;
    msec = (num - (unsigned int)num) * 1000;

    sprintf(dest, "%02d:%02d.%03d", min, sec, msec);
}



