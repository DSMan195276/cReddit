# Makefile for registering info about the system we're compiling on.
O_FLAG:=Y

ifndef QUIETLY
	QUIETLY:=@
endif
ECHO:=$(QUIETLY)echo
DIE:=$(QUIETLY)edit -1

MKDIR_CHECK:= $(shell mkdir --version 2>/dev/null)
ifdef MKDIR_CHECK
	MKDIR:=$(QUIETLY)mkdir -p
endif

RM_CHECK:= $(shell rm --version 2>/dev/null)
ifdef RM_CHECK
	RM:=$(QUIETLY)rm
endif

RMDIR_CHECK:= $(shell rm --version 2>/dev/null)
ifdef RMDIR_CHECK
	RMDIR:=$(QUIETLY)rm -fr
endif

GCC_CHECK:= $(shell gcc --version 2>/dev/null)
ifdef GCC_CHECK
	CC:=$(QUIETLY)gcc
endif

LD_CHECK:= $(shell ld --version 2>/dev/null)
ifdef LD_CHECK
	LD:=$(QUIETLY)ld
endif

INSTALL_CHECK:= $(shell install --version 2>/dev/null)
ifdef INSTALL_CHECK
	INSTALL:=$(QUIETLY)install
endif

ifdef STATIC
	AR_CHECK:= $(shell ar --version 2>/dev/null)
	ifdef AR_CHECK
		AR:=$(QUIETLY)ar
	endif

	ifndef AR
		$(ECHO) "Error: Not able to find an archiver for compiling the static library"
		$(DIE)
	endif
endif

ifndef MKDIR
	$(ECHO) "Error: Not able to find a command to create directories"
	$(DIE)
endif

ifndef RM
	$(ECHO) "Error: Unable to find a command to remove files"
	$(DIE)
endif

ifndef RMDIR
	$(ECHO) "Error: Unable to find a command to remove directories"
	$(DIE)
endif

ifndef CC
	$(ECHO) "Error: Not able to detect a C compiler"
	$(DIE)
endif

ifndef LD
	$(ECHO) "Error: Not able to detect a linker"
	$(DIE)
endif

ifndef INSTALL
	$(ECHO) "Error: Not able to detect an installer"
	$(DIE)
endif

UNAME=$(shell uname -r)
ifeq ('Darwin','$(UNAME)')
	F_MACOSX=yes
endif
ifeq ('Linux','$(UNAME)')
	F_LINUX=yes
endif


