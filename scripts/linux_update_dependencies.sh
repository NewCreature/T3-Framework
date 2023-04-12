#!/bin/bash

function remake_dir() {
  rm -rf $1
  mkdir $1
}

if [ "$#" -ne 1 ]; then
  echo "Usage: linux_set_up_dependencies <path>"
  exit 1
fi

START_PATH=$(pwd)

mkdir -p $1
cd $1

# DUMB
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

# OGG
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

# Vorbis
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

# Opus
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

# Opusfile
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

# libiconv (needed by FLAC)
if [ ! -d "libiconv" ];
then
  remake_dir libiconv
fi
cd libiconv
wget https://ftp.gnu.org/gnu/libiconv/libiconv-1.17.tar.gz
tar xzf libiconv-1.17.tar.gz
cd libiconv-1.17
./configure --enable-static=yes --enable-shared=no
make
sudo make install

# FLAC
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

# minimp3
if [ ! -d "minimp3" ];
then
  git clone https://github.com/lieff/minimp3.git
fi
cd minimp3
git pull
sudo cp minimp3.h /usr/local/include
sudo cp minimp3_ex.h /usr/local/include
cd ..

# zlib
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

# libpng
if [ ! -d "libpng" ];
then
  git clone git://git.code.sf.net/p/libpng/code
  mv code libpng
fi
cd libpng
git pull
remake_dir _build
cd _build
cmake .. -DPNG_PREFIX=t3f_ -DPNG_EXECUTABLES=OFF -DPNG_SHARED=OFF -DPNG_TESTS=OFF -DZLIB_INCLUDE_DIR=/usr/local/include -DZLIB_LIBRARY_RELEASE=/usr/local/lib/libz.a -DPNG_BUILD_ZLIB=ON
make
sudo make install
cd ..
cd ..

# libjpeg
if [ ! -d "libjpeg-turbo" ];
then
  git clone https://github.com/libjpeg-turbo/libjpeg-turbo.git
fi
cd libjpeg-turbo
git pull
remake_dir _build
cd _build
cmake .. -DENABLE_SHARED=OFF
make
sudo make install
cd ..
cd ..

# libwebp
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

# FreeType
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

# PhysFS
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

# Allegro
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

# return to original location
cd $START_PATH
