#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/player.h"

#include "../include/window.h"

int main(int argc, char *argv[])
{
    return window_run(argc - optind, argv);
}
