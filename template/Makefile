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

all:
	$(MAKE) -f makefile.$(SYSTEM)
	$(MAKE) -C editors/avatar_editor -f makefile.$(SYSTEM)
	$(MAKE) -C editors/commentary_editor -f makefile.$(SYSTEM)
	$(MAKE) -C editors/course_editor -f makefile.$(SYSTEM)

clean:
	$(MAKE) -f makefile.$(SYSTEM) clean

clean_all:
	$(MAKE) -f makefile.$(SYSTEM) clean
	$(MAKE) -C editors/avatar_editor -f makefile.$(SYSTEM) clean
	$(MAKE) -C editors/commentary_editor -f makefile.$(SYSTEM) clean
	$(MAKE) -C editors/course_editor -f makefile.$(SYSTEM) clean

package:
	$(MAKE) -f makefile.$(SYSTEM) clean
	$(MAKE) -f makefile.$(SYSTEM) package

debian_package:
	$(MAKE) -f makefile.$(SYSTEM)_static debian_package
