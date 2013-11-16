
CREDDIT_CFLAGS:=$(PROJCFLAGS)
CREDDIT_LDFLAGS:=$(PROJLDFILES) -L$(BUILD_DIR) -lreddit
ifdef F_MACOSX
	CREDDIT_LDFLAGS+=-lncurses
else
	CREDDIT_LDFLAGS+=-lncursesw
endif

ifdef STATIC
    CREDDIT_LDFLAGS+=`curl-config --cflags` `curl-config --libs`
endif

EXECUTABLE_NAME:=creddit
CREDDIT_DIR_CMP:=$(BUILD_DIR)/src
CREDDIT_DIR:=src

CREDDIT_CFLAGS+=-I'$(CREDDIT_DIR)/include'

EXECUTABLE_FULL:=$(BUILD_DIR)/$(EXECUTABLE_NAME)

CREDDIT_SOURCES:=$(patsubst $(CREDDIT_DIR)/%,%,$(wildcard $(CREDDIT_DIR)/*.c))
CREDDIT_OBJECTS:=$(patsubst %,$(CREDDIT_DIR_CMP)/%,$(CREDDIT_SOURCES:.c=.o))

$(CREDDIT_DIR_CMP): | $(BUILD_DIR)
	$(ECHO) " MKDIR   $(CREDDIT_DIR_CMP)"
	$(MKDIR) $(CREDDIT_DIR_CMP)

include ./$(CREDDIT_DIR)/objects/objects.mk
include ./$(CREDDIT_DIR)/screens/screens.mk

creddit: $(EXECUTABLE_FULL)

$(EXECUTABLE_FULL): libreddit $(OBJS_COMBINE) $(SCRS_COMBINE) $(CREDDIT_OBJECTS) | $(CREDDIT_DIR_CMP)
	$(ECHO) " CCLD    $(EXECUTABLE_FULL)"
	$(CC) $(OBJS_COMBINE) $(SCRS_COMBINE) $(CREDDIT_OBJECTS) $(CREDDIT_CFLAGS) $(CREDDIT_LDFLAGS) -o $(EXECUTABLE_FULL)

creddit_clean:
	$(ECHO) " RMDIR   $(CREDDIT_DIR_CMP)"
	$(RMDIR) $(CREDDIT_DIR_CMP)

creddit_install: $(EXECUTABLE_FULL) | $(PREFIX)/bin
	$(ECHO) " INSTALL $(EXECUTABLE_FULL)"
	$(INSTALL) -m 0755 $(EXECUTABLE_FULL) $(PREFIX)/bin/

$(CREDDIT_DIR_CMP)/%.o: $(CREDDIT_DIR)/%.c | $(CREDDIT_DIR_CMP)
	$(ECHO) " CC      $@"
	$(CC) $(CREDDIT_CFLAGS) -c $< -o $@

COMPILE_TARGETS+=$(EXECUTABLE_FULL)
CLEAN_TARGETS+=creddit_clean
INSTALL_TARGETS+=creddit_install

