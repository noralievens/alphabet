################################################################################
# @author       : Arno Lievens (arnolievens@gmail.com)
# @date         : Sunday Jun 27, 2021 09:37:42 CEST
# @file         : Makefile
# @brief        : makefile
# @copyright    :

TARGET 			= alphabet
NAME 		 	= Alphabet
DESCRIPTION  	= audioplayer
VERSION      	= 0.1
AUTHOR       	= arnolievens@gmail.com
DATE         	= July 2021


################################################################################
# Dirs
#
PREFIX          = /usr/local
SRC_DIR     	= src
INC_DIR     	= include
DATA_DIR    	= share
BIN_DIR     	= bin
BUILD_DIR   	= build
MAN_DIR     	= $(DATA_DIR)/man
MAN_SECTION 	= man1
DESKTOP_DIR     = $(DATA_DIR)/applications
ICON_DIR 		= $(DATA_DIR)/icons


################################################################################
# Build
#
SOURCES        := $(shell find $(SRC_DIR) -name *.c)
OBJECTS        := $(addprefix $(BUILD_DIR)/,$(notdir $(SOURCES:.c=.o)))
LIBS         	= $(shell pkg-config --libs gtk+-3.0 mpv libebur128 )
LIBS           += $(shell pkg-config --libs libavformat libavutil sndfile)
LIBS		   += -lm
INCLUDES     	= $(shell pkg-config --cflags gtk+-3.0 mpv libebur128)
INCLUDES       += $(shell pkg-config --cflags libavformat libavutil sndfile)
INCLUDES	   += -I/usr/include/mpv

CC           	= gcc

CFLAGS		    = -DVERSION=\"$(VERSION)\"
CFLAGS		   += -DDEBUG -g
CFLAGS         += -std=gnu99 -pedantic -Wextra -Wall -Wundef -Wshadow
CFLAGS		   += -Wpointer-arith -Wcast-align -Wstrict-prototypes
CFLAGS		   += -Wstrict-overflow=5 -Wwrite-strings -Wcast-qual
CFLAGS		   += -Wswitch-default
CFLAGS		   += -Wunreachable-code
# CFLAGS		   += -Wswitch-enum
# CFLAGS		   += -Wconversion

LDFLAGS         =


################################################################################
# Doc
#
DOXY_CFG 		= doxygen.cfg
DOC_DIR  		= doc


################################################################################
# Debian dpkg
#
DEB_PKG 		= $(TARGET).deb
dpkg: PREFIX    = $(TARGET)/usr/local

define DEBIAN_CONTROL
Package: $(TARGET)
Version: $(VERSION)
section: custom
priority: optional
architecture: all
essential: no
installed-size: 1024
Depends: $(DEPENDENCIES)
maintainer: $(AUTHOR)
description: $(DESCRIPTION)
endef
export DEBIAN_CONTROL


################################################################################
# MacOs .app
#
APP_PKG 		= $(NAME).app
MAC_DIR         = macosx
OS  			= $(shell sh -c 'uname 2> /dev/null || echo Unknown_OS')

ifeq ($(OS),Darwin)
	CFLAGS     += -DMAC_INTEGRATION
	LIBS       += $(shell pkg-config --libs gtk-mac-integration-gtk3)
	INCLUDES   += $(shell pkg-config --cflags gtk-mac-integration-gtk3)
endif


################################################################################
# Targets
#
.PHONY: clean install uninstall deb app doc man homebrew

all: $(BIN_DIR)/$(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCLUDES)

$(BIN_DIR)/$(TARGET): $(OBJECTS)
	mkdir -p $(BIN_DIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) -I$(INCLUDES) $(LIBS)

man:
	mkdir -p ./$(MAN_DIR)/$(MAN_SECTION)
	printf "%s\n%s\n%s\n\n" \
		"% $(shell echo $(TARGET) | tr a-z A-Z)(1) $(TARGET) $(VERSION)" \
		"% $(AUTHOR)" \
		"% $(DATE)" > ./$(DOC_DIR)/$(TARGET).tmp
	pandoc      $(DOC_DIR)/$(TARGET).tmp \
		        $(DOC_DIR)/$(TARGET).md \
				-s -t man -o \
		        $(MAN_DIR)/$(MAN_SECTION)/$(TARGET).1
	rm -fv      $(DOC_DIR)/$(TARGET).tmp
	@printf "\e[0;32m%s\e[0m\n" "generated manpage in ./$(MAN_DIR)"

doc:
	mkdir -p $(DOC_DIR)
	doxygen $(DOXY_CFG)
	@printf "\e[0;32m%s\e[0m\n" "generated doxygen docs in ./$(DOC_DIR)"

install: all
	mkdir -pv   $(PREFIX)/$(BIN_DIR)
	cp -fv      $(BIN_DIR)/$(TARGET) \
	            $(PREFIX)/$(BIN_DIR)/$(TARGET)
	chmod 755   $(PREFIX)/$(BIN_DIR)/$(TARGET)
	mkdir -pv   $(PREFIX)/$(MAN_DIR)/$(MAN_SECTION)
	cp -fv      $(MAN_DIR)/$(MAN_SECTION)/$(TARGET).1 \
				$(PREFIX)/$(MAN_DIR)/$(MAN_SECTION)/$(TARGET).1
	chmod 644   $(PREFIX)/$(MAN_DIR)/$(MAN_SECTION)/$(TARGET).1
	mkdir -pv   $(PREFIX)/$(ICON_DIR)/$(TARGET)
	cp -fv      $(ICON_DIR)/* \
	            $(PREFIX)/$(ICON_DIR)/
ifeq ($(OS),Linux)
	mkdir -pv   $(PREFIX)/$(DESKTOP_DIR)
	cp -fv      $(DESKTOP_DIR)/$(TARGET).desktop \
				$(PREFIX)/$(DESKTOP_DIR)
endif
	@printf "\e[0;32m%s\e[0m\n" "installed $(TARGET) into $(PREFIX)"

uninstall:
	rm -fv      $(PREFIX)/$(BIN_DIR)/$(TARGET)
	rm -fv      $(PREFIX)/$(MAN_DIR)/$(MAN_SECTION)/$(TARGET).1
	rm -frv     $(PREFIX)/$(ICON_DIR)/$(TARGET)
ifeq ($(OS),Linux)
	rm -fr      $(PREFIX)/$(DESKTOP_DIR)/$(TARGET).desktop
endif
	@printf "\e[0;32m%s\e[0m\n" "uninstalled $(TARGET) from $(PREFIX)"

deb: install
	mkdir -p 	$(TARGET)/DEBIAN
	@echo "$$DEBIAN_CONTROL" > $(TARGET)/DEBIAN/control
	dpkg-deb --build $(TARGET) $(DEB_PKG)
	@printf "\e[0;32m%s\e[0m\n" "built $(DEB_PKG)"

$(MAC_DIR)/$(APPNAME).icns: $(ICON_DIR)/$(TARGET).png $(BIN_DIR)/$(TARGET)
	rm -rf      $(MAC_DIR)/$(NAME).iconset
	mkdir -v    $(MAC_DIR)/$(NAME).iconset
	OF=$(MAC_DIR)/$(NAME).iconset/icon_
	sips -z 16 16     $(ICON_DIR)/$(TARGET).png --out $(OF)16x16.png
	sips -z 32 32     $(ICON_DIR)/$(TARGET).png --out $(OF)_16x16@2x.png
	sips -z 32 32     $(ICON_DIR)/$(TARGET).png --out $(OF)_32x32.png
	sips -z 64 64     $(ICON_DIR)/$(TARGET).png --out $(OF)_32x32@2x.png
	sips -z 128 128   $(ICON_DIR)/$(TARGET).png --out $(OF)_128x128.png
	sips -z 256 256   $(ICON_DIR)/$(TARGET).png --out $(OF)_128x128@2x.png
	sips -z 256 256   $(ICON_DIR)/$(TARGET).png --out $(OF)_256x256.png
	sips -z 512 512   $(ICON_DIR)/$(TARGET).png --out $(OF)_256x256@2x.png
	sips -z 512 512   $(ICON_DIR)/$(TARGET).png --out $(OF)_512x512.png
	sips -z 1024 1024 $(ICON_DIR)/$(TARGET).png --out $(OF)_512x512@2x.png
	iconutil -c icns -o $(MAC_DIR)/$(NAME).icns macosx/$(NAME).iconset
	rm -rv      $(MAC_DIR)/$(NAME).iconset

app: $(MAC_DIR)/$(APPNAME).icns
	rm -fr      $(APP_PKG)
	mkdir -p    $(APP_PKG)/Contents/Resources
	echo "APPLAlphabet" \
		      > $(APP_PKG)/Contents/PkgInfo
	cp -fv		$(MAC_DIR)/Info.plist \
				$(APP_PKG)/Contents/
	cp -fv		$(MAC_DIR)/$(NAME).icns \
				$(APP_PKG)/Contents/Resources/
	cp -fv		$(BIN_DIR)/$(TARGET) \
				$(APP_PKG)/Contents/MacOs/
	@printf "\e[0;32m%s\e[0m\n" "built $(APP_PKG)"

homebrew:
	brew install -- \
				pkg-config \
				gtk+3 \
				mpv \
				libebur128 \
				gtk-mac-integration
	@printf "\e[0;32m%s\e[0m\n" "homebrew installed dependencies"

apt:
	apt install -- \
				pkg-config \
				libgtk-3-dev \
				libmpv-dev \
				libebur128-dev \
				libavformat-dev \
				libavutil-dev
	@printf "\e[0;32m%s\e[0m\n" "apt installed dependencies"

clean:
	rm -fvr     ./$(BUILD_DIR)
	rm -fvr     ./$(BIN_DIR)
	rm -fvr     ./$(MAN_DIR)
	rm -fvr     ./$(TARGET)
	rm -fv      ./$(TARGET).deb
	rm -fvr     ./$(APP_PKG)
	@printf "\e[0;32m%s\e[0m\n" "cleaned $(TARGET)"
