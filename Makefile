# Automatically selects the correct makefile for the current platform,
# assumes user is building from source for their own use (won't select static
# or cross-compile makefiles)

UNAME=$(shell uname)
ifneq (,$(findstring Linux,$(UNAME)))
SYSTEM=linux
else
ifneq (,$(findstring Darwin,$(UNAME)))
SYSTEM=macosx
else
ifneq (,$(findstring MINGW,$(UNAME)))
SYSTEM=mingw
else
$(warning Not sure which system you are on, assuming Linux)
$(warning   (uname says: '$(UNAME)'))
SYSTEM=linux
endif
endif
endif

top:
	$(MAKE) -f makefile.$(SYSTEM)

library:
	$(MAKE) -f makefile.$(SYSTEM) library

clean:
	$(MAKE) -f makefile.$(SYSTEM) clean

project:
	@$(MAKE) -f makefile.project
