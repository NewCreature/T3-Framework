# Automatically selects the correct makefile for the current platform,
# assumes user is building from source for their own use (won't select static
# or cross-compile makefiles)

ifeq ($(OS),Windows_NT)
 UNAME=$(shell uname)
 ifneq (,$(findstring MINGW,$(UNAME)))
  SYSTEM=msys
 else
  SYSTEM=mingw
 endif
else
 UNAME=$(shell uname)
 ifneq (,$(findstring Linux,$(UNAME)))
  SYSTEM=linux
 else
  ifneq (,$(findstring Darwin,$(UNAME)))
   SYSTEM=macos
  else
   ifneq (,$(findstring MINGW,$(UNAME)))
    SYSTEM=mingw_cross
   else
    $(warning Not sure which system you are on, assuming Linux)
    $(warning   (uname says: '$(UNAME)'))
    SYSTEM=linux
   endif
  endif
 endif
endif

top:
	$(MAKE) -f ../scripts/makefile.$(SYSTEM) -I ../scripts all

clean:
	$(MAKE) -f ../scripts/makefile.$(SYSTEM) -I ../scripts clean

release:
	$(MAKE) -f ../scripts/makefile.$(SYSTEM) -I ../scripts release

source_package:
	$(MAKE) -f ../scripts/makefile.$(SYSTEM) -I ../scripts source_package

source_only_package:
	$(MAKE) -f ../scripts/makefile.$(SYSTEM) -I ../scripts source_only_package

binary_package:
	$(MAKE) -f ../scripts/makefile.$(SYSTEM) -I ../scripts binary_package

debian_package:
	$(MAKE) -f ../scripts/makefile.debian -I ../scripts debian_package

android_package:
	$(MAKE) -f ../scripts/makefile.android -I ../scripts android_package

android_bundle:
	$(MAKE) -f ../scripts/makefile.android -I ../scripts android_bundle

android_install:
	$(MAKE) -f ../scripts/makefile.android -I ../scripts android_install

android_debug:
	$(MAKE) -f ../scripts/makefile.android -I ../scripts android_debug

mac_i386:
	$(MAKE) -f ../scripts/makefile.$(SYSTEM) -I ../scripts mac_i386

mac_x86_64:
	$(MAKE) -f ../scripts/makefile.$(SYSTEM) -I ../scripts mac_x86_64

mac_arm64:
	$(MAKE) -f ../scripts/makefile.$(SYSTEM) -I ../scripts mac_arm64

mac_universal_bundle:
	$(MAKE) -f ../scripts/makefile.macos -I ../scripts mac_universal_bundle

mac_bundle:
	$(MAKE) -f ../scripts/makefile.macos -I ../scripts mac_bundle

iphone_package:
	$(MAKE) -f ../scripts/makefile.iphone -I ../scripts iphone_package

flatpak:
	$(MAKE) -f ../scripts/makefile.flatpak -I ../scripts flatpak

demo:
	$(MAKE) -f ../scripts/makefile.$(SYSTEM) -I ../scripts demo

demo_gp:
	$(MAKE) -f ../scripts/makefile.$(SYSTEM) -I ../scripts demo_gp

makeicon:
	$(MAKE) -f ../scripts/makefile.$(SYSTEM) -I ../scripts ../windows/makeicon

reset_config:
	$(MAKE) -f ../scripts/makefile.$(SYSTEM) -I ../scripts reset_config

finish:
	$(MAKE) -f ../scripts/makefile.$(SYSTEM) -I ../scripts finish

install:
	$(MAKE) -f ../scripts/makefile.$(SYSTEM) -I ../scripts install

uninstall:
	$(MAKE) -f ../scripts/makefile.$(SYSTEM) -I ../scripts uninstall

install_flatpak:
	$(MAKE) -f ../scripts/makefile.$(SYSTEM) -I ../scripts install_flatpak

data_package:
	$(MAKE) -f ../scripts/makefile.$(SYSTEM) -I ../scripts data_package

steam_packages:
	$(MAKE) -f ../scripts/makefile.$(SYSTEM) -I ../scripts steam_packages

help:
	@echo "\nRun 'make' without arguments to build en executable for the current plaform."
	@echo "\nTargets available on all platforms:\n"
	@echo "  install:             Install application onto the system (Linux only)"
	@echo "  uninstall:           Uninstall appplication from the system (Linux only)"
	@echo "  clean:               Clean up files produced by the build process."
	@echo "  release:             Update 'docs/changelog' based on data in 'changes.txt'."
	@echo "  source_package:      Create source code package with bundled data."
	@echo "  source_only_package: Create source code package without bundled data."
	@echo "  binary_package:      Create data and binary executable standalone package\n                       suitable for distrubition."
	@echo "  steam_packages:      Create data and binary packages suitable for distribution\n                       through Steam."
