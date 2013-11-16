
TUI_CFLAGS:=$(OBJS_CFLAGS)
TUI_LDFLAGS:=$(OBJS_LDFLAGS)

TUI_DIR:=$(OBJS_DIR)/tui
TUI_DIR_CMP:=$(OBJS_DIR_CMP)/tui

TUI_COMBINE:=$(TUI_DIR_CMP)/tui.o

TUI_SOURCES:=$(patsubst $(TUI_DIR)/%,%,$(wildcard $(TUI_DIR)/*.c))
TUI_OBJECTS:=$(patsubst %,$(TUI_DIR_CMP)/%,$(TUI_SOURCES:.c=.o))

$(TUI_DIR_CMP): | $(OBJS_DIR_CMP)
	$(ECHO) " MKDIR   $(TUI_DIR_CMP)"
	$(MKDIR) $(TUI_DIR_CMP)

tui: $(TUI_COMBINE)

tui_clean:
	$(ECHO) " RMDIR   $(TUI_DIR_CMP)"
	$(RMDIR) $(TUI_DIR_CMP)

$(TUI_COMBINE): $(TUI_OBJECTS)
	$(ECHO) " LD      $(TUI_COMBINE)"
	$(LD) -r $(TUI_OBJECTS) -o $(TUI_COMBINE)

$(TUI_DIR_CMP)/%.o: $(TUI_DIR)/%.c | $(TUI_DIR_CMP)
	$(ECHO) " CC      $@"
	$(CC) $(TUI_CFLAGS) -c $< -o $@

COMPILE_TARGETS+=$(TUI_COMBINE)
CLEAN_TARGETS+=tui_clean

