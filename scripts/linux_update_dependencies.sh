#!/bin/bash

# set up variable defaults
START_PATH=$(pwd)
X86_SDK=MacOSX10.13.sdk
ARM_SDK=MacOSX11.sdk
BUILD_CMAKE=0
BUILD_DUMB=1
BUILD_OGG=1
BUILD_VORBIS=1
BUILD_OPUS=1
BUILD_OPUSFILE=1
BUILD_LIBICONV=1
BUILD_FLAC=1
BUILD_MINIMP3=1
BUILD_ZLIB=1
BUILD_LIBPNG=1
BUILD_LIBJPEG=1
BUILD_LIBWEBP=1
BUILD_FREETYPE=1
BUILD_PHYSFS=1
BUILD_ALLEGRO=1
OS_ID=unknown

function detect_os() {
  if [ -f /etc/os-release ]; then
    . /etc/os-release
    OS_ID=$ID
    echo "Detected OS: $OS_ID."
  else
    OS_ID=unknown
    echo "Could not detect OS."
  fi
}

function setup_ubuntu() {
  sudo apt install build-essential
  sudo apt install make
  sudo apt install cmake
  sudo apt install git
}

function setup_fedora() {
  sudo dnf install gcc
  sudo dnf install g++
  sudo dnf install make
  sudo dnf install cmake
  sudo dnf install git
  sudo dnf install libX11-devel
  sudo dnf install xorg-x11-server-devel
  sudo dnf install libXcursor-devel
  sudo dnf install freeglut-devel
  sudo dnf install gtk3-devel
  sudo dnf install pulseaudio-libs-devel
  sudo dnf install alsa-lib-devel
}

function setup_arch() {
  echo "Arch"
}

function remake_dir() {
  rm -rf $1
  mkdir $1
}

function disable_all() {
  BUILD_DUMB=0
  BUILD_OGG=0
  BUILD_VORBIS=0
  BUILD_OPUS=0
  BUILD_OPUSFILE=0
  BUILD_LIBICONV=0
  BUILD_FLAC=0
  BUILD_MINIMP3=0
  BUILD_ZLIB=0
  BUILD_LIBPNG=0
  BUILD_LIBJPEG=0
  BUILD_LIBWEBP=0
  BUILD_FREETYPE=0
  BUILD_PHYSFS=0
  BUILD_ALLEGRO=0
}

if [ "$#" -le 0 ]; then
  echo "Usage: linux_update_dependencies <path> <arguments>"
  exit 1
fi

detect_os
if [[ "$OS_ID" == "ubuntu" ]]; then
  setup_ubuntu
elif [[ "$OS_ID" == "fedora" ]]; then
  setup_fedora
elif [[ "$OS_ID" == "arch" ]]; then
  setup_arch
fi

# check arguments
for arg in "$@";
do
  if [ $arg = --build-cmake ]; then
    BUILD_CMAKE=1
  fi
  if [ $arg = --dumb_only ]; then
    disable_all
    BUILD_DUMB=1
  fi
  if [ $arg = --ogg_only ]; then
    disable_all
    BUILD_OGG=1
  fi
  if [ $arg = --vorbis_only ]; then
    disable_all
    BUILD_VORBIS=1
  fi
  if [ $arg = --opus_only ]; then
    disable_all
    BUILD_OPUS=1
  fi
  if [ $arg = --opusfile_only ]; then
    disable_all
    BUILD_OPUSFILE=1
  fi
  if [ $arg = --libiconv_only ]; then
    disable_all
    BUILD_LIBICONV=1
  fi
  if [ $arg = --flac_only ]; then
    disable_all
    BUILD_FLAC=1
  fi
  if [ $arg = --minimp3_only ]; then
    disable_all
    BUILD_MINIMP3=1
  fi
  if [ $arg = --zlib_only ]; then
    disable_all
    BUILD_ZLIB=1
  fi
  if [ $arg = --libpng_only ]; then
    disable_all
    BUILD_LIBPNG=1
  fi
  if [ $arg = --libjpeg_only ]; then
    disable_all
    BUILD_LIBJPEG=1
  fi
  if [ $arg = --libwebp_only ]; then
    disable_all
    BUILD_LIBWEBP=1
  fi
  if [ $arg = --freetype_only ]; then
    disable_all
    BUILD_FREETYPE=1
  fi
  if [ $arg = --physfs_only ]; then
    disable_all
    BUILD_PHYSFS=1
  fi
  if [ $arg = --allegro_only ]; then
    disable_all
    BUILD_ALLEGRO=1
  fi
done

mkdir -p $1
cd $1

# CMake (most dependencies are built with it)
if [ $BUILD_CMAKE -eq 1 ]; then
  if [ ! -d "CMake" ];
  then
    git clone https://github.com/Kitware/CMake.git
  fi
  cd CMake
  git pull
  ./bootstrap -- -DCMAKE_USE_OPENSSL=OFF
  make
  sudo make install
  cd ..
fi

# DUMB
if [ $BUILD_DUMB -eq 1 ]; then
  echo "Updating DUMB..."
  if [ ! -d "dumb" ];
  then
    git clone https://github.com/kode54/dumb.git
  fi
  cd dumb
  git pull
  remake_dir _build
  cd _build
  cmake .. -DBUILD_ALLEGRO4=OFF -DBUILD_EXAMPLES=OFF
  make
  sudo make install
  cd ..
  cd ..
fi

# OGG
if [ $BUILD_OGG -eq 1 ]; then
  echo "Updating libogg..."
  if [ ! -d "ogg" ];
  then
    git clone https://github.com/xiph/ogg.git
  fi
  cd ogg
  git pull
  remake_dir _build
  cd _build
  cmake ..
  make
  sudo make install
  cd ..
  cd ..
fi

# Vorbis
if [ $BUILD_VORBIS -eq 1 ]; then
  echo "Updating libvorbis..."
  if [ ! -d "vorbis" ];
  then
    git clone https://github.com/xiph/vorbis.git
  fi
  cd vorbis
  git pull
  remake_dir _build
  cd _build
  cmake .. -DBUILD_TESTING=OFF
  make
  sudo make install
  cd ..
  cd ..
fi

# Opus
if [ $BUILD_OPUS -eq 1 ]; then
  echo "Updating Opus..."
  if [ ! -d "opus" ];
  then
    git clone https://github.com/xiph/opus.git
  fi
  cd opus
  git pull
  remake_dir _build
  cd _build
  cmake ..
  make
  sudo make install
  cd ..
  cd ..
fi

# Opusfile
if [ $BUILD_OPUSFILE -eq 1 ]; then
  echo "Updating Opusfile..."
  if [ ! -d "opusfile" ];
  then
    git clone https://github.com/xiph/opusfile.git
  fi
  cd opusfile
  git pull
  remake_dir _build
  cd _build
  cmake .. -DOP_DISABLE_DOCS=ON -DOP_DISABLE_EXAMPLES=ON -DOP_DISABLE_HTTP=ON
  make
  sudo make install
  cd ..
  cd ..
fi

# libiconv (needed by FLAC)
if [ $BUILD_LIBICONV -eq 1 ]; then
  echo "Updating libiconv..."
  if [ ! -d "libiconv" ];
  then
    remake_dir libiconv
  fi
  cd libiconv
  wget https://ftp.gnu.org/gnu/libiconv/libiconv-1.18.tar.gz
  tar xzf libiconv-1.17.tar.gz
  cd libiconv-1.17
  ./configure --enable-static=yes --enable-shared=no
  make
  sudo make install
  cd ..
  cd ..
fi

# FLAC
if [ $BUILD_FLAC -eq 1 ]; then
  echo "Updating FLAC..."
  if [ ! -d "flac" ];
  then
    git clone https://github.com/xiph/flac.git
  fi
  cd flac
  git pull
  remake_dir _build
  cd _build
  cmake .. -DBUILD_DOCS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_PROGRAMS=OFF -DBUILD_TESTING=OFF -DINSTALL_MANPAGES=OFF
  make
  sudo make install
  cd ..
  cd ..
fi

# minimp3
if [ $BUILD_MINIMP3 -eq 1 ]; then
  echo "Updating minimp3..."
  if [ ! -d "minimp3" ];
  then
    git clone https://github.com/lieff/minimp3.git
  fi
  cd minimp3
  git pull
  sudo cp minimp3.h /usr/local/include
  sudo cp minimp3_ex.h /usr/local/include
  cd ..
fi

# zlib
if [ $BUILD_ZLIB -eq 1 ]; then
  echo "Updating zlib..."
  if [ ! -d "zlib" ];
  then
    git clone https://github.com/madler/zlib.git
  fi
  cd zlib
  git pull
  remake_dir _build
  cd _build
  cmake ..
  make
  sudo make install
  sudo rm -f /usr/local/lib/libz.so*
  cd ..
  cd ..
fi

# libpng
if [ $BUILD_LIBPNG -eq 1 ]; then
  echo "Updating libpng..."
  if [ ! -d "libpng" ];
  then
    git clone git://git.code.sf.net/p/libpng/code
    mv code libpng
  fi
  cd libpng
  git pull
  remake_dir _build
  cd _build
  cmake .. -DPNG_PREFIX=t3f_ -DPNG_EXECUTABLES=OFF -DPNG_SHARED=OFF -DPNG_TESTS=OFF -DZLIB_INCLUDE_DIR=/usr/local/include -DZLIB_LIBRARY_RELEASE=/usr/local/lib/libz.a -DZLIB_ROOT=/usr/local
  make
  sudo make install
  cd ..
  cd ..
fi

# libjpeg
if [ $BUILD_LIBJPEG -eq 1 ]; then
  echo "Updating libjpeg..."
  if [ ! -d "libjpeg-turbo" ];
  then
    git clone https://github.com/libjpeg-turbo/libjpeg-turbo.git
  fi
  cd libjpeg-turbo
  git pull
  remake_dir _build
  cd _build
  cmake .. -DENABLE_SHARED=OFF -DCMAKE_INSTALL_PREFIX=/usr/local
  make
  sudo make install
  cd ..
  cd ..
fi

# libwebp
if [ $BUILD_LIBWEBP -eq 1 ]; then
  echo "Updating libwebp..."
  if [ ! -d "libwebp" ];
  then
    git clone https://chromium.googlesource.com/webm/libwebp
  fi
  cd libwebp
  git pull
  remake_dir _build
  cd _build
  cmake ..
  make
  sudo make install
  cd ..
  cd ..
fi

# FreeType
if [ $BUILD_FREETYPE -eq 1 ]; then
  echo "Updating libfreetype..."
  if [ ! -d "freetype" ];
  then
    git clone https://gitlab.freedesktop.org/freetype/freetype.git
  fi
  cd freetype
  git pull
  remake_dir _build
  cd _build
  cmake .. -DFT_DISABLE_BROTLI=ON -DFT_DISABLE_BZIP2=ON -DFT_DISABLE_HARFBUZZ=ON -DFT_DISABLE_PNG=ON -DFT_DISABLE_ZLIB=ON
  make
  sudo make install
  cd ..
  cd ..
fi

# PhysFS
if [ $BUILD_PHYSFS -eq 1 ]; then
  echo "Updating PhysFS..."
  if [ ! -d "physfs" ];
  then
    git clone https://github.com/icculus/physfs.git
  fi
  cd physfs
  git pull
  remake_dir _build
  cd _build
  cmake .. -DPHYSFS_BUILD_SHARED=OFF -DPHYSFS_BUILD_DOCS=OFF
  make
  sudo make install
  cd ..
  cd ..
fi

# Packaged Allegro dependencies
sudo apt install libgl1-mesa-dev libglu1-mesa-dev libgtk-3-dev libpulse-dev libxext-dev libxxf86vm-dev libxrandr-dev libxinerama-dev libxpm-dev

# Allegro
if [ $BUILD_ALLEGRO -eq 1 ]; then
  echo "Updating Allegro..."
  if [ ! -d "allegro5" ];
  then
    git clone https://github.com/liballeg/allegro5.git
  fi
  cd allegro5
  git pull
  remake_dir _build
  cd _build
  cmake .. -DSHARED=OFF -DWANT_DEMO=OFF -DWANT_DOCS=OFF -DWANT_EXAMPLES=OFF -DWANT_NATIVE_IMAGE_LOADER=OFF -DWANT_TESTS=NO -DZLIB_INCLUDE_DIR=/usr/local/include -DZLIB_LIBRARY_RELEASE=/usr/local/lib/libz.a -DWANT_IMAGE_FREEIMAGE=NO -DWANT_IMAGE_PNG=NO -DWANT_OPENAL=NO
  make
  sudo make install
  cd ..
  cd ..
fi

# return to original location
cd $START_PATH
