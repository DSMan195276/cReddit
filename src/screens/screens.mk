
SCRS_CFLAGS:=$(CREDDIT_CFLAGS)
SCRS_LDFLAGS:=$(CREDDIT_LDFLAGS)

SCRS_DIR:=$(CREDDIT_DIR)/screens
SCRS_DIR_CMP:=$(CREDDIT_DIR_CMP)/screens

SCRS_COMBINE:=$(CREDDIT_DIR_CMP)/screens.o

SCRS_SOURCES:=$(patsubst $(SCRS_DIR)/%,%,$(wildcard $(SCRS_DIR)/*.c))
SCRS_OBJECTS:=$(patsubst %,$(SCRS_DIR_CMP)/%,$(SCRS_SOURCES:.c=.o))

$(SCRS_DIR_CMP): | $(CREDDIT_DIR_CMP)
	$(ECHO) " MKDIR $(SCRS_DIR_CMP)"
	$(MKDIR) $(SCRS_DIR_CMP)

screens: $(SCRS_COMBINE)

screens_clean:
	$(ECHO) " RM $(SCRS_DIR_CMP)"
	$(RM) -fr $(SCRS_DIR_CMP)

$(SCRS_COMBINE): $(SCRS_OBJECTS)
	$(ECHO) " LD $(SCRS_COMBINE)"
	$(LD) -r $(SCRS_OBJECTS) -o $(SCRS_COMBINE)

$(SCRS_DIR_CMP)/%.o: $(SCRS_DIR)/%.c | $(SCRS_DIR_CMP)
	$(ECHO) " CC $@"
	$(CC) $(SCRS_CFLAGS) -c $< -o $@

COMPILE_TARGETS+=$(SCRS_COMBINE)
CLEAN_TARGETS+=screens_clean
