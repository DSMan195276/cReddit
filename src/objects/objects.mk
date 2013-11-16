
OBJS_CFLAGS:=$(CREDDIT_CFLAGS)
OBJS_LDFLAGS:=$(CREDDIT_LDFLAGS)

OBJS_DIR:=$(CREDDIT_DIR)/objects
OBJS_DIR_CMP:=$(CREDDIT_DIR_CMP)/objects

OBJS_COMBINE:=$(OBJS_DIR_CMP)/objects.o

OBJS_SOURCES:=$(patsubst $(OBJS_DIR)/%,%,$(wildcard $(OBJS_DIR)/*.c))
OBJS_OBJECTS:=$(patsubst %,$(OBJS_DIR_CMP)/%,$(OBJS_SOURCES:.c=.o))

$(OBJS_DIR_CMP): | $(CREDDIT_DIR_CMP)
	$(ECHO) " MKDIR   $(OBJS_DIR_CMP)"
	$(MKDIR) $(OBJS_DIR_CMP)

include ./$(OBJS_DIR)/tui/tui.mk

objects: $(OBJS_COMBINE)

objects_clean:
	$(ECHO) " RMDIR   $(OBJS_DIR_CMP)"
	$(RMDIR) $(OBJS_DIR_CMP)

$(OBJS_COMBINE): $(TUI_COMBINE) $(OBJS_OBJECTS)
	$(ECHO) " LD      $(OBJS_COMBINE)"
	$(LD) -r $(TUI_COMBINE) $(OBJS_OBJECTS) -o $(OBJS_COMBINE)

$(OBJS_DIR_CMP)/%.o: $(OBJS_DIR)/%.c | $(OBJS_DIR_CMP)
	$(ECHO) " CC      $@"
	$(CC) $(OBJS_CFLAGS) -c $< -o $@

COMPILE_TARGETS+=$(OBJS_COMBINE)
CLEAN_TARGETS+=objects_clean

