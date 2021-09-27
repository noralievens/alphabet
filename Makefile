################################################################################
# @author       : Arno Lievens (arnolievens@gmail.com)
# @date         : 08/09/2021
# @file         : Makefile
# @copyright    : Copyright (c) 2021 Arno Lievens
#
TARGET 			= alphabet
NAME 		 	= Alphabet
DESCRIPTION  	= audioplayer
VERSION      	= 0.1
AUTHOR       	= arnolievens@gmail.com
DATE         	= 08/09/2021

include config.mk

################################################################################
# Build
#
SOURCES         = $(shell find $(SRC_DIR) -name *.c)
HEADERS         = $(shell find $(INC_DIR) -name *.h)
OBJECTS         = $(addprefix $(BUILD_DIR)/,$(notdir $(SOURCES:.c=.o)))
LIBS           +=
INCLUDES       +=

CC           	= gcc
CFLAGS		    = -DVERSION=\"$(VERSION)\"
# CFLAGS		   += -DDEBUG -g
CFLAGS         += -std=gnu99 -pedantic -Wextra -Wall -Wundef -Wshadow
CFLAGS		   += -Wpointer-arith -Wcast-align -Wstrict-prototypes
CFLAGS		   += -Wstrict-overflow=5 -Wwrite-strings -Wcast-qual
CFLAGS		   += -Wswitch-default
CFLAGS		   += -Wunreachable-code
# CFLAGS		   += -Wswitch-enum
# CFLAGS		   += -Wconversion

CTAGS  	        = ctags
CTAGSFLAGS      =


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
DOC_DIR 		= doc
DESKTOP_DIR     = $(DATA_DIR)/applications
ICON_DIR 		= $(DATA_DIR)/icons


################################################################################
# Doxy
#
DOXY_CFG 		= $(DOC_DIR)/doxygen.cfg
DOXY_DIR  		= doxy


################################################################################
# Debian .deb
#
DEB_PKG 		= $(TARGET).deb
deb: PREFIX     = $(TARGET)/usr/local

define DEBIAN_CONTROL
Package: $(TARGET)
Version: $(VERSION)
section: custom
priority: optional
architecture: all
essential: no
installed-size: 1024
Depends: $(DEB_DEPS)
maintainer: $(AUTHOR)
description: $(DESCRIPTION)
endef
export DEBIAN_CONTROL


################################################################################
# MacOs .app
#
APP_PKG 		= $(NAME).app
MAC_DIR         = macosx
APP_ICON        = $(MAC_DIR)/$(NAME).iconset


################################################################################
# Gitignore

define GIT_IGNORE
$(TARGET)/
$(DEB_PKG)
$(APP_PKG)
*.dmg

$(BIN_DIR)/
$(BUILD_DIR)/
$(DOXY_DIR)/
$(MAN_DIR)/

compile_commands.json
.ccls-cache
tags
$(MAC_DIR)/Contents/Resources/$(NAME).icns
endef
export GIT_IGNORE


################################################################################
# Targets
#
.PHONY: 	all init ctags gitignore man doxy install uninstall \
			deb app apt brew clean

all: $(BIN_DIR)/$(TARGET)

init:
	mkdir -pv 	$(SRC_DIR)
	mkdir -pv 	$(INC_DIR)
	mkdir -pv 	$(DOC_DIR)
	mkdir -pv 	$(DATA_DIR)
	doxygen -g  $(DOXY_CFG)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCLUDES)

$(BIN_DIR)/$(TARGET): $(OBJECTS)
	mkdir -p $(BIN_DIR)
	$(CC) $(OBJECTS) -o $@ -I$(INCLUDES) $(LIBS)

ctags: $(HEADERS) $(SOURCES)
	$(CTAGS) $(CTAGSFLAGS) $(HEADERS) $(SOURCES)

gitignore:
	@echo "$$GIT_IGNORE" > .gitignore
	@printf "\e[0;32m%s\e[0m\n" "copied defaults to .gitignore"

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

doxy:
	mkdir -p $(DOXY_DIR)
	doxygen $(DOXY_CFG)
	@printf "\e[0;32m%s\e[0m\n" "generated doxygen docs in ./$(DOXY_DIR)"

install: all man
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

$(MAC_DIR)/$(APPNAME).icns: $(ICON_DIR)/$(TARGET).png
	rm -rf      $(MAC_DIR)/$(NAME).iconset
	mkdir -pv   $(MAC_DIR)/$(NAME).iconset
	mkdir -pv 	$(MAC_DIR)/Contents/Resources/
	sips -z 16 16     $(ICON_DIR)/$(TARGET).png --out $(APP_ICON)/icon_16x16.png
	sips -z 32 32     $(ICON_DIR)/$(TARGET).png --out $(APP_ICON)/icon_16x16@2x.png
	sips -z 32 32     $(ICON_DIR)/$(TARGET).png --out $(APP_ICON)/icon_32x32.png
	sips -z 64 64     $(ICON_DIR)/$(TARGET).png --out $(APP_ICON)/icon_32x32@2x.png
	sips -z 128 128   $(ICON_DIR)/$(TARGET).png --out $(APP_ICON)/icon_128x128.png
	sips -z 256 256   $(ICON_DIR)/$(TARGET).png --out $(APP_ICON)/icon_128x128@2x.png
	sips -z 256 256   $(ICON_DIR)/$(TARGET).png --out $(APP_ICON)/icon_256x256.png
	sips -z 512 512   $(ICON_DIR)/$(TARGET).png --out $(APP_ICON)/icon_256x256@2x.png
	sips -z 512 512   $(ICON_DIR)/$(TARGET).png --out $(APP_ICON)/icon_512x512.png
	sips -z 1024 1024 $(ICON_DIR)/$(TARGET).png --out $(APP_ICON)/icon_512x512@2x.png
	iconutil -c icns -o $(MAC_DIR)/Contents/Resources/$(NAME).icns $(APP_ICON)
	rm -rv      $(MAC_DIR)/$(NAME).iconset

app: $(MAC_DIR)/$(APPNAME).icns $(BIN_DIR)/$(TARGET)
	rm -fr      $(APP_PKG)
	rm -f 		$(APP_PKG).dmg
	cp -fvr 	$(MAC_DIR) \
				$(APP_PKG)
	cp -fv		$(BIN_DIR)/$(TARGET) \
				$(APP_PKG)/Contents/MacOs/$(TARGET)-bin
	dylibbundler -od -b -x \
				$(APP_PKG)/Contents/MacOS/$(TARGET)-bin -d \
				$(APP_PKG)/Contents/Resources/lib
	create-dmg 	$(APP_PKG) .
	@printf "\e[0;32m%s\e[0m\n" "built $(APP_PKG)"

brew:
	brew install $(BREW_DEPS)
	@printf "\e[0;32m%s\e[0m\n" "homebrew installed dependencies"

apt:
	apt install $(APT_DEPS)
	@printf "\e[0;32m%s\e[0m\n" "apt installed dependencies"

clean:
	rm -fvr     ./$(BUILD_DIR)
	rm -fvr     ./$(BIN_DIR)
	rm -fvr 	./$(DOXY_DIR)
	rm -fvr     ./$(MAN_DIR)
	rm -fvr     ./$(TARGET)
	rm -fv      ./$(TARGET).deb
	rm -fvr     ./$(APP_PKG)
	rm -fv      ./tags
	@printf "\e[0;32m%s\e[0m\n" "cleaned $(TARGET)"

help:
	@echo '$(TARGET) Makefile help'
	@echo
	@echo 'usage: make <TARGET>'
	@echo '  all         compile and link'
	@echo '  init        create default directories'
	@echo '  ctags       create ctags for VI editor'
	@echo '  gitignore   create default .gitignore file'
	@echo '  man         convert doc/$(TARGET).md to manpage'
	@echo '  doxy        build doxygen documentation'
	@echo '  install     install in $(PREXIF)'
	@echo '  uninstall   install from $(PREXIF)'
	@echo '  deb         builb .deb package'
	@echo '  app         build .app macos bundle'
	@echo '  apt         install dependencies with apt'
	@echo '  brew        install dependencies with homebrew'
	@echo '  clean       clean build, bin, doxy, man'
	@echo '  help        print this message'
