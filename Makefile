################################################################################
# @author       : Arno Lievens (arnolievens@gmail.com)
# @date         : 08/09/2021
# @file         : Makefile
# @copyright    : Copyright (c) 2021 Arno Lievens
#
CONFIG          = config.mk
include $(CONFIG)

################################################################################
# Build
#
SOURCES         = $(shell find "$(SRC_DIR)" -name *.c)
HEADERS         = $(shell find "$(INC_DIR)" -name *.h)
OBJECTS         = $(addprefix $(BUILD_DIR)/,$(notdir $(SOURCES:.c=.o)))
LIBS           +=
INCLUDES       +=

CC              = gcc
CFLAGS         += -DVERSION=\"$(VERSION)\" -DID=\"$(ID)\"
CFLAGS         += -DDEBUG -g
CFLAGS         += -std=gnu11 -pedantic -Wextra -Wall -Wundef -Wshadow
CFLAGS         += -Wpointer-arith -Wcast-align -Wstrict-prototypes
CFLAGS         += -Wstrict-overflow=5 -Wwrite-strings
CFLAGS         += -Wswitch-default
CFLAGS         += -Wunreachable-code
CFLAGS         += -Wdiscarded-qualifiers -Wcast-qual
CFLAGS         += -Wconversion

LDFLAGS        +=

CTAGS           = ctags
CTAGSFLAGS      =


################################################################################
# Dirs
#
PREFIX          = /usr/local
SRC_DIR         = src
INC_DIR         = include
DATA_DIR        = share
BIN_DIR         = bin
BUILD_DIR       = build
DIST_DIR        = dist
MAN_DIR         = $(DATA_DIR)/man
MAN_SECTION     = man1
DOC_DIR         = doc
DESKTOP_DIR     = $(DATA_DIR)/applications
ICON_DIR        = $(DATA_DIR)/icons/hicolor/scalable/apps
ICON_NAME       = $(ID).svg


################################################################################
# Doxy
#
DOXY_CFG        = $(DOC_DIR)/doxygen.cfg
DOXY_DIR        = doxy


################################################################################
# Desktop entry
#
#
DESKTOP_ENTRY   = $(DESKTOP_DIR)/$(TARGET).desktop

define DESKTOP_ENTRY_CONTENT
[Desktop Entry]
Type=Application
Version=$(VERSION)
Name=$(NAME)
Comment=$(DESCRIPTION)
Exec=$(TARGET) %F
Icon=$(ID)
Terminal=$(DESKTOP_TERM)
Keywords=$(DESKTOP_KEY)
MimeType=$(DESKTOP_MIME)
Categories=$(DESKTOP_CAT)
$(DESKTOP_LOCAL)
endef
export DESKTOP_ENTRY_CONTENT


################################################################################
# Debian .deb
#
DEB_PKG         = $(DIST_DIR)/$(TARGET).deb
deb: PREFIX     = $(DIST_DIR)/$(TARGET)/usr/local
deb: SIZE       = $(shell [ -d "$(DIST_DIR)/$(TARGET)" ] && \
	               du -s "$(DIST_DIR)/$(TARGET)" | cut -f 1)

define DEBIAN_CONTROL
Package: $(TARGET)
Version: $(VERSION)
section: custom
priority: optional
architecture: all
essential: no
installed-size: $(SIZE)
Depends: $(DEB_DEPS)
maintainer: $(AUTHOR)
description: $(DESCRIPTION)
endef
export DEBIAN_CONTROL


################################################################################
# Appimage
#
APPIMG_PKG      = $(DIST_DIR)/$(TARGET).appimage
appimg: PREFIX  = $(DIST_DIR)/$(TARGET)/usr


################################################################################
# MacOs .app
#
APP_PKG         = $(DIST_DIR)/$(NAME).app
MAC_DIR         = macosx
APP_ICON        = $(MAC_DIR)/$(NAME).iconset


################################################################################
# Gitignore

define GIT_IGNORE
$(DEB_PKG)
$(APP_PKG)
*.dmg
$(DESKTOP_ENTRY)

$(BIN_DIR)/
$(BUILD_DIR)/
$(DIST_DIR)/
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
.PHONY: all init ctags gitignore man doxy desktop install uninstall deb app apt brew clean

all: $(BIN_DIR)/$(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCLUDES)

$(BIN_DIR)/$(TARGET): $(OBJECTS)
	mkdir -p $(BIN_DIR)
	$(CC) $(OBJECTS) -o $@ -I$(INCLUDES) $(LIBS) $(LDFLAGS)

ctags: $(HEADERS) $(SOURCES)
	$(CTAGS) $(CTAGSFLAGS) $(HEADERS) $(SOURCES)

init:
	mkdir -pv    $(SRC_DIR)
	mkdir -pv    $(INC_DIR)
	mkdir -pv    $(DOC_DIR)
	mkdir -pv    $(DATA_DIR)
	mkdir -pv    $(DIST_DIR)
	doxygen -g  $(DOXY_CFG)
	touch        $(CONFIG)
	@printf "\e[0;32m%s\e[0m\n" "init complete"

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

desktop:
	mkdir -pv   $(DESKTOP_DIR)
	echo "$$DESKTOP_ENTRY_CONTENT" > $(DESKTOP_ENTRY)
	@printf "\e[0;32m%s\e[0m\n" "updated desktop entry in $(DESKTOP_DIR)"

install: desktop man all
	mkdir -pv   $(PREFIX)/$(BIN_DIR)
	cp -fv      $(BIN_DIR)/$(TARGET) \
	            $(PREFIX)/$(BIN_DIR)/$(TARGET)
	chmod 755   $(PREFIX)/$(BIN_DIR)/$(TARGET)
	mkdir -pv   $(PREFIX)/$(MAN_DIR)/$(MAN_SECTION)
	cp -fv      $(MAN_DIR)/$(MAN_SECTION)/$(TARGET).1 \
	            $(PREFIX)/$(MAN_DIR)/$(MAN_SECTION)/$(TARGET).1
	chmod 644   $(PREFIX)/$(MAN_DIR)/$(MAN_SECTION)/$(TARGET).1
	mkdir -pv   $(PREFIX)/$(ICON_DIR)
	cp -rfv     $(ICON_DIR)/$(ICON_NAME) \
	            $(PREFIX)/$(ICON_DIR)
ifeq ($(OS),Linux)
	mkdir -pv   $(PREFIX)/$(DESKTOP_DIR)
	cp -fv      $(DESKTOP_DIR)/$(TARGET).desktop \
	            $(PREFIX)/$(DESKTOP_DIR)
endif
	@printf "\e[0;32m%s\e[0m\n" "installed $(TARGET) into $(PREFIX)"

uninstall:
	rm -fv      $(PREFIX)/$(BIN_DIR)/$(TARGET)
	rm -fv      $(PREFIX)/$(MAN_DIR)/$(MAN_SECTION)/$(TARGET).1
	rm -frv     $(PREFIX)/$(ICON_DIR)/$(ICON_NAME)
ifeq ($(OS),Linux)
	rm -fr      $(PREFIX)/$(DESKTOP_DIR)/$(TARGET).desktop
endif
	@printf "\e[0;32m%s\e[0m\n" "uninstalled $(TARGET) from $(PREFIX)"

deb: install
	rm -fvr     $(DEB_PKG)
	mkdir -p    $(DIST_DIR)/$(TARGET)/DEBIAN
	@echo "$$DEBIAN_CONTROL" > $(DIST_DIR)/$(TARGET)/DEBIAN/control
	dpkg-deb --build $(DIST_DIR)/$(TARGET) $(DEB_PKG)
	rm -rv      $(DIST_DIR)/$(TARGET)
	@printf "\e[0;32m%s\e[0m\n" "built $(DEB_PKG)"

appimg: install
	mkdir -pv   $(DIST_DIR)/$(TARGET)
	linuxdeploy --appdir $(DIST_DIR)/$(TARGET) --output appimage
	mv -v       *.AppImage $(DIST_DIR)/
	@printf "\e[0;32m%s\e[0m\n" "built $(APPIMG_PKG)"

$(MAC_DIR)/$(APPNAME).icns: $(ICON_DIR)/$(ICON_NAME)
	svg2icns    $<
	mv -v       $(ICON_NAME:.svg=.icns) \
	            $(MAC_DIR)/Contents/Resources/$(TARGET).icns

app: $(MAC_DIR)/$(APPNAME).icns $(BIN_DIR)/$(TARGET)
	rm -fr      $(APP_PKG)
	mkdir -pv 	$(DIST_DIR)
	cp -fvr     $(MAC_DIR) \
	            $(APP_PKG)
	cp -fv      $(BIN_DIR)/$(TARGET) \
	            $(APP_PKG)/Contents/MacOs/$(TARGET)-bin
	dylibbundler -od -b -x \
	            $(APP_PKG)/Contents/MacOS/$(TARGET)-bin -d \
	            $(APP_PKG)/Contents/Resources/lib
	rm -fv 		$(DIST_DIR)/*.dmg
	create-dmg  $(APP_PKG) \
		        $(DIST_DIR)
	@printf "\e[0;32m%s\e[0m\n" "built $(APP_PKG)"

brew:
	brew install $(BREW_DEPS)
	@echo "please install svg2icns https://github.com/magnusviri/svg2icns.git"
	@printf "\e[0;32m%s\e[0m\n" "homebrew installed dependencies"

apt:
	apt install $(APT_DEPS)
	@printf "\e[0;32m%s\e[0m\n" "apt installed dependencies"

clean:
	rm -fvr     ./$(BUILD_DIR)
	rm -fvr     ./$(BIN_DIR)
	rm -fvr     ./$(DOXY_DIR)
	rm -fvr     ./$(MAN_DIR)
	rm -fvr     ./$(TARGET)
	rm -fv      ./$(TARGET).deb
	rm -fvr     ./$(APP_PKG)
	rm -fvr     ./$(DIST_DIR)
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
	@echo '  desktop     build desktop entry'
	@echo '  install     install in $(PREXIF)'
	@echo '  uninstall   uninstall from $(PREXIF)'
	@echo '  deb         builb .deb package'
	@echo '  appimg      builb .AppImage package'
	@echo '  app         build .app macos bundle'
	@echo '  apt         install dependencies with apt'
	@echo '  brew        install dependencies with homebrew'
	@echo '  clean       clean build, bin, doxy, man'
	@echo '  help        print this message'
