#!/bin/bash

function merge_libs() {
  lipo -create $1/$3 $2/$3 -output $1/$3
}

function remake_dir() {
  rm -rf $1
  mkdir $1
}

if [ "$#" -ne 1 ]; then
  echo "Usage: ios_update_dependencies <path>"
  exit 1
fi

START_PATH=$(pwd)
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/iPhoneOS.sdk

mkdir -p $1
cd $1

# DUMB
if [ ! -d "dumb" ];
then
  git clone https://github.com/kode54/dumb.git
fi
cd dumb
git pull
remake_dir _build_ios
cd _build_ios
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_INSTALL_PREFIX=/opt/local_ios -DBUILD_ALLEGRO4=OFF -DBUILD_EXAMPLES=OFF
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
remake_dir _build_ios
cd _build_ios
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_INSTALL_PREFIX=/opt/local_ios
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
remake_dir _build_ios
cd _build_ios
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_INSTALL_PREFIX=/opt/local_ios -DBUILD_TESTING=OFF
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
remake_dir _build_ios
cd _build_ios
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_INSTALL_PREFIX=/opt/local_ios
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
remake_dir _build_ios
cd _build_ios
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_INSTALL_PREFIX=/opt/local_ios -DOP_DISABLE_DOCS=ON -DOP_DISABLE_EXAMPLES=ON -DOP_DISABLE_HTTP=ON
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
curl https://ftp.gnu.org/gnu/libiconv/libiconv-1.17.tar.gz --output libiconv-1.17.tar.gz
tar xzf libiconv-1.17.tar.gz
cd libiconv-1.17
./configure CFLAGS="-isysroot /Library/Developer/CommandLineTools/SDKs/iPhoneOS.sdk" --prefix=/opt/local_ios --enable-shared=no --enable-static=yes
make clean
make
sudo make install
cd ..
cd ..

# FLAC
if [ ! -d "flac" ];
then
  git clone https://github.com/xiph/flac.git
fi
cd flac
git pull
remake_dir _build_ios
cd _build_ios
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_INSTALL_PREFIX=/opt/local_ios -DBUILD_DOCS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_PROGRAMS=OFF -DBUILD_TESTING=OFF -DINSTALL_MANPAGES=OFF -DIconv_INCLUDE_DIR=/opt/local_ios/include -DIconv_LIBRARY=/opt/local_ios/lib/libiconv.a
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
sudo cp minimp3.h /opt/local_ios/include
sudo cp minimp3_ex.h /opt/local_ios/include
cd ..

# zlib
if [ ! -d "zlib" ];
then
  git clone https://github.com/madler/zlib.git
fi
cd zlib
git pull
remake_dir _build_ios
cd _build_ios
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_INSTALL_PREFIX=/opt/local_ios
make
sudo make install
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
remake_dir _build_ios
cd _build_ios
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_INSTALL_PREFIX=/opt/local_ios -DPNG_TOOLS=OFF -DPNG_SHARED=OFF -DPNG_FRAMEWORK=OFF -DPNG_TESTS=OFF -DZLIB_INCLUDE_DIR=/opt/local_ios/include -DZLIB_LIBRARY_RELEASE=/opt/local_ios/lib/libz.a -DZLIB_ROOT=/opt/local_ios
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
remake_dir _build_ios
cd _build_ios
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_INSTALL_PREFIX=/opt/local_ios -DENABLE_SHARED=OFF
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
remake_dir _build_ios
cd _build_ios
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_INSTALL_PREFIX=/opt/local_ios -DZLIB_INCLUDE_DIR=/opt/local_ios/include -DZLIB_LIBRARY_RELEASE=/opt/local_ios/lib/libz.a -DJPEG_INCLUDE_DIR=/opt/local_ios/include -DJPEG_LIBRARY_RELEASE=/opt/local_ios/lib/libjpeg.a -DPNG_INCLUDE_DIR=/opt/local_ios/include -DPNG_LIBRARY_RELEASE=/opt/local_ios/lib/libpng.a -DWEBP_BUILD_CWEBP=OFF -DWEBP_BUILD_DWEBP=OFF
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
remake_dir _build_ios
cd _build_ios
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_INSTALL_PREFIX=/opt/local_ios -DFT_DISABLE_BROTLI=ON -DFT_DISABLE_BZIP2=ON -DFT_DISABLE_HARFBUZZ=ON -DFT_DISABLE_PNG=ON -DFT_DISABLE_ZLIB=ON
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
remake_dir _build_ios
cd _build_ios
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_INSTALL_PREFIX=/opt/local_ios -DPHYSFS_BUILD_SHARED=OFF -DPHYSFS_BUILD_DOCS=OFF
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
rm -rf deps
ln -s /opt/local_ios deps
remake_dir _build_ios
cd _build_ios
cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-iphone.cmake -DIOS_PLATFORM="iphoneos" -DCMAKE_INSTALL_PREFIX=/opt/local_ios -DWANT_EXAMPLES=off -DWANT_DEMO=off -DWANT_TESTS=off -DZLIB_INCLUDE_DIR=/opt/local_ios/include -DZLIB_LIBRARY_RELEASE=/opt/local_ios/lib/libz.a
make
sudo make install
cd ..
cd ..

# return to original location
cd $START_PATH
