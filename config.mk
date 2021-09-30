################################################################################
# @author       : Arno Lievens (arnolievens@gmail.com)
# @date         : 24/09/2021
# @file         : config.mk
# @copyright    : Copyright (c) 2021 Arno Lievens
#
TARGET          = alphabet
NAME            = Alphabet
DESCRIPTION     = "Music player"
VERSION         = 0.11
AUTHOR          = arnolievens@gmail.com
DATE            = 08/09/2021

OS              = $(shell sh -c 'uname 2> /dev/null || echo Unknown_OS')

################################################################################
# Libs
#
LIBS            = $(shell pkg-config --libs gtk+-3.0 mpv libebur128)
LIBS           += $(shell pkg-config --libs libavformat libavutil sndfile)
LIBS           += -lm
ifeq ($(OS),Darwin)
    LIBS       += $(shell pkg-config --libs gtk-mac-integration-gtk3)
endif


################################################################################
# Includes
#
INCLUDES        = $(shell pkg-config --cflags gtk+-3.0 mpv libebur128)
INCLUDES       += $(shell pkg-config --cflags libavformat libavutil sndfile)
INCLUDES       += -I/usr/include/mpv
ifeq ($(OS),Darwin)
    INCLUDES   += $(shell pkg-config --cflags gtk-mac-integration-gtk3)
endif


################################################################################
# Cflags
#
CFLAGS         += -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED
ifeq ($(OS),Darwin)
    CFLAGS     += -DMAC_INTEGRATION
endif


################################################################################
# Desktop entry (csv) - optionally add local options (Key=value)
#
DESKTOP_TERM    =false
DESKTOP_KEY     =$(TARGET);media;player;audio;
DESKTOP_MIME    =application/ogg;application/x-ogg;audio/aac;audio/x-aac;\
				 audio/wav;audio/flac;audio/aiff;audio/x-aiff;audio/m4a;\
				 audio/x-m4a;
DESKTOP_CAT     =Audio;Player;
DESKTOP_LOCAL   =


################################################################################
# Debian .deb
#
DEB_DEPS        = libgtk-3-0, libmpv1, libebur128-1,
DEB_DEPS       += libavformat58, libavutil56, libsndfile1


################################################################################
# Apt
#
APT_DEPS        = libgtk-3-dev libmpv-dev libebur128-dev
APT_DEPS       += libavformat-dev libavutil-dev libsndfile1-dev


################################################################################
# Brew
#
BREW_DEPS       = pkg-config gtk+3 mpv libebur128 gtk-mac-integration ffmpeg
