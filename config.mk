################################################################################
# Os
#
OS  			= $(shell sh -c 'uname 2> /dev/null || echo Unknown_OS')

################################################################################
# Libs
#
LIBS         	= $(shell pkg-config --libs gtk+-3.0 mpv libebur128 )
LIBS           += $(shell pkg-config --libs libavformat libavutil sndfile)
LIBS		   += -lm
ifeq ($(OS),Darwin)
	LIBS       += $(shell pkg-config --libs gtk-mac-integration-gtk3)
endif


################################################################################
# Includes
#
INCLUDES     	= $(shell pkg-config --cflags gtk+-3.0 mpv libebur128)
INCLUDES       += $(shell pkg-config --cflags libavformat libavutil sndfile)
INCLUDES	   += -I/usr/include/mpv
ifeq ($(OS),Darwin)
	INCLUDES   += $(shell pkg-config --cflags gtk-mac-integration-gtk3)
endif


################################################################################
# Cflags
#
ifeq ($(OS),Darwin)
	CFLAGS     += -DMAC_INTEGRATION
endif


################################################################################
# Debian .deb
#
DEB_DEPS        = libgtk-3-0, libmpv1, libebur128-1,
DEB_DEPS       += libavformat56, libavutil56, libsndfile1


################################################################################
# Apt
#
APT_DEPS        = libgtk-3-dev libmpv-dev libebur128-dev
APT_DEPS       += libavformat-dev libavutil-dev libsndfile1-dev


################################################################################
# Brew
#
BREW_DEPS       = pkg-config gtk+3 mpv libebur128 gtk-mac-integration ffmpeg
