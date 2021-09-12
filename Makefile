################################################################################
# @author      : Arno Lievens (arnolievens@gmail.com)
# @file        : Makefile
# @created     : Sunday Jun 27, 2021 09:37:42 CEST
################################################################################

TARGET       = alphabet
DESCRIPTION  = audioplayer
VERSION      = 0.1
AUTHOR       = arnolievens@gmail.com
DATE         = July 2021

DEST_DIR     = usr
PREFIX       = local

SRC_DIR      = src
INC_DIR      = include
DATA_DIR     = share
BIN_DIR      = bin
BUILD_DIR    = build
MAN_DIR      = man
MAN_SECTION  = man1

# DEPENDENCIES = libmpv


SOURCES     := $(shell find $(SRC_DIR) -name *.c)
OBJECTS     := $(addprefix $(BUILD_DIR)/,$(notdir $(SOURCES:.c=.o)))

LIBS         = $(shell pkg-config --libs gtk+-3.0 ) $(shell pkg-config --libs mpv) -lm
INCLUDES     = $(shell pkg-config --cflags gtk+-3.0 ) $(shell pkg-config --cflags mpv) -I/usr/include/mpv

CC           = gcc

CFLAGS       = -g -std=gnu99 -pedantic -Wextra -Wall -Wundef -Wshadow \
			   -Wpointer-arith -Wcast-align -Wstrict-prototypes \
			   -Wstrict-overflow=5 -Wwrite-strings -Wcast-qual \
			   -Wswitch-default \
			   -Wunreachable-code \
			   -DVERSION=\"$(VERSION)\"
			   # -Wswitch-enum \
			   # -Wconversion \

LDFLAGS      =

# debian dpkg control file
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

.PHONY: clean install uninstall dpkg

all: $(BIN_DIR)/$(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCLUDES)

$(BIN_DIR)/$(TARGET): $(OBJECTS)
	mkdir -p $(BIN_DIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) -I$(INCLUDES) $(LIBS)

man:
	mkdir -p $(MAN_DIR)/$(MAN_SECTION)
	printf "%s\n%s\n%s\n\n" \
		"% $(shell echo $(TARGET) | tr a-z A-Z)(1) $(TARGET) $(VERSION)" \
		"% $(AUTHOR)" \
		"% $(DATE)" > doc/$(TARGET).tmp
	pandoc doc/$(TARGET).tmp doc/$(TARGET).md -s -t man -o $(MAN_DIR)/$(MAN_SECTION)/$(TARGET).1
	rm -f doc/$(TARGET).tmp
	./buildhelp.sh doc/$(TARGET).md

clean:
	rm -rf ./$(BUILD_DIR)
	rm -rf ./$(BIN_DIR)
	rm -rf ./$(MAN_DIR)
	rm -rf ./$(TARGET)
	rm  -f ./$(TARGET).deb

install: all
	mkdir -p /$(DEST_DIR)/$(PREFIX)/$(BIN_DIR)
	cp -f ./$(BIN_DIR)/$(TARGET) /$(DEST_DIR)/$(PREFIX)/$(BIN_DIR)/$(TARGET)
	chmod 755 /$(DEST_DIR)/$(PREFIX)/$(BIN_DIR)/$(TARGET)
	mkdir -p /$(DEST_DIR)/$(PREFIX)/$(DATA_DIR)/$(MAN_DIR)/$(MAN_SECTION)
	cp -f ./$(MAN_DIR)/$(MAN_SECTION)/$(TARGET).1 /$(DEST_DIR)/$(PREFIX)/$(DATA_DIR)/$(MAN_DIR)/$(MAN_SECTION)/$(TARGET).1
	chmod 644 /$(DEST_DIR)/$(PREFIX)/$(DATA_DIR)/$(MAN_DIR)/$(MAN_SECTION)/$(TARGET).1
	mkdir -p /$(DEST_DIR)/$(PREFIX)/$(DATA_DIR)/$(TARGET)/
	cp -f ./$(DATA_DIR)/* /$(DEST_DIR)/$(PREFIX)/$(DATA_DIR)/$(TARGET)/

uninstall:
	rm  -f /$(DEST_DIR)/$(PREFIX)/$(BIN_DIR)/$(TARGET)
	rm  -f /$(DEST_DIR)/$(PREFIX)/$(DATA_DIR)/$(MAN_DIR)/$(MAN_SECTION)/$(TARGET).1
	rm -rf /$(DEST_DIR)/$(PREFIX)/$(DATA_DIR)/$(TARGET)/

dpkg: all
	mkdir -p $(TARGET)/$(DEST_DIR)/$(PREFIX)/$(BIN_DIR)
	cp -f ./$(BIN_DIR)/$(TARGET) $(TARGET)/$(DEST_DIR)/$(PREFIX)/$(BIN_DIR)/$(TARGET)
	chmod 755 $(TARGET)/$(DEST_DIR)/$(PREFIX)/$(BIN_DIR)/$(TARGET)
	mkdir -p $(TARGET)/$(DEST_DIR)/$(PREFIX)/$(DATA_DIR)/$(MAN_DIR)/$(MAN_SECTION)
	cp -f ./$(MAN_DIR)/$(MAN_SECTION)/$(TARGET).1 $(TARGET)/$(DEST_DIR)/$(PREFIX)/$(DATA_DIR)/$(MAN_DIR)/$(MAN_SECTION)/$(TARGET).1
	chmod 644 $(TARGET)/$(DEST_DIR)/$(PREFIX)/$(DATA_DIR)/$(MAN_DIR)/$(MAN_SECTION)/$(TARGET).1
	mkdir -p $(TARGET)/$(DEST_DIR)/$(PREFIX)/$(DATA_DIR)/$(TARGET)/
	cp -f ./$(DATA_DIR)/* $(TARGET)/$(DEST_DIR)/$(PREFIX)/$(DATA_DIR)/$(TARGET)/
	mkdir -p $(TARGET)/DEBIAN
	@echo "$$DEBIAN_CONTROL" > $(TARGET)/DEBIAN/control
	dpkg-deb --build $(TARGET) $(TARGET).deb
