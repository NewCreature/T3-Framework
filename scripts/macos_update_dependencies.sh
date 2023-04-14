#!/bin/bash

function merge_libs() {
  lipo -create $1/$3 $2/$3 -output $1/$3
}

function remake_dir() {
  rm -rf $1
  mkdir $1
}

if [ "$#" -ne 1 ]; then
  echo "Usage: macos_update_dependencies <path>"
  exit 1
fi

START_PATH=$(pwd)

mkdir -p $1
cd $1

# DUMB
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX10.13.sdk
if [ ! -d "dumb" ];
then
  git clone https://github.com/kode54/dumb.git
fi
cd dumb
git pull
remake_dir _build_x86
cd _build_x86
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=i386\;x86_64 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6 -DBUILD_ALLEGRO4=OFF -DBUILD_EXAMPLES=OFF
make
cd ..
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX11.sdk
remake_dir _build_arm
cd _build_arm
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 -DBUILD_ALLEGRO4=OFF -DBUILD_EXAMPLES=OFF
make
merge_libs ./ ../_build_x86 libdumb.a
sudo make install
cd ..
cd ..

# OGG
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX10.13.sdk
if [ ! -d "ogg" ];
then
  git clone https://github.com/xiph/ogg.git
fi
cd ogg
git pull
remake_dir _build_x86
cd _build_x86
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=i386\;x86_64 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6
make
cd ..
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX11.sdk
remake_dir _build_arm
cd _build_arm
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0
make
merge_libs ./ ../_build_x86 libogg.a
sudo make install
cd ..
cd ..

# Vorbis
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX10.13.sdk
if [ ! -d "vorbis" ];
then
  git clone https://github.com/xiph/vorbis.git
fi
cd vorbis
git pull
remake_dir _build_x86
cd _build_x86
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=i386\;x86_64 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6 -DBUILD_TESTING=OFF
make
cd ..
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX11.sdk
remake_dir _build_arm
cd _build_arm
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 -DBUILD_TESTING=OFF
make
merge_libs lib ../_build_x86/lib libvorbis.a
merge_libs lib ../_build_x86/lib libvorbisenc.a
merge_libs lib ../_build_x86/lib libvorbisfile.a
sudo make install
cd ..
cd ..

# Opus
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX10.13.sdk
if [ ! -d "opus" ];
then
  git clone https://github.com/xiph/opus.git
fi
cd opus
git pull
remake_dir _build_x86
cd _build_x86
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=i386\;x86_64 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6
make
cd ..
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX11.sdk
remake_dir _build_arm
cd _build_arm
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 -DOPUS_USE_NEON=OFF
make
merge_libs ./ ../_build_x86 libopus.a
sudo make install
cd ..
cd ..

# Opusfile
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX10.13.sdk
if [ ! -d "opusfile" ];
then
  git clone https://github.com/xiph/opusfile.git
fi
cd opusfile
git pull
remake_dir _build_x86
cd _build_x86
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=i386\;x86_64 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6 -DOP_DISABLE_DOCS=ON -DOP_DISABLE_EXAMPLES=ON -DOP_DISABLE_HTTP=ON
make
cd ..
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX11.sdk
remake_dir _build_arm
cd _build_arm
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 -DOP_DISABLE_DOCS=ON -DOP_DISABLE_EXAMPLES=ON -DOP_DISABLE_HTTP=ON
make
merge_libs ./ ../_build_x86 libopusfile.a
sudo make install
cd ..
cd ..

# FLAC
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX10.13.sdk
if [ ! -d "flac" ];
then
  git clone https://github.com/xiph/flac.git
fi
cd flac
git pull
remake_dir _build_x86
cd _build_x86
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=i386\;x86_64 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6 -DBUILD_DOCS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_PROGRAMS=OFF -DBUILD_TESTING=OFF -DINSTALL_MANPAGES=OFF
make
cd ..
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX11.sdk
remake_dir _build_arm
cd _build_arm
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 -DBUILD_DOCS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_PROGRAMS=OFF -DBUILD_TESTING=OFF -DINSTALL_MANPAGES=OFF
make
merge_libs src/libFLAC ../_build_x86/src/libFLAC libFLAC.a
merge_libs src/libFLAC++ ../_build_x86/src/libFLAC++ libFLAC++.a
sudo make install
cd ..
cd ..

# zlib
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX10.13.sdk
if [ ! -d "zlib" ];
then
  git clone https://github.com/madler/zlib.git
fi
cd zlib
git pull
remake_dir _build_x86
cd _build_x86
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=i386\;x86_64 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6
make
cd ..
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX11.sdk
remake_dir _build_arm
cd _build_arm
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0
make
merge_libs ./ ../_build_x86 libz.a
sudo make install
sudo rm -rf /usr/local/lib/libz*.dylib /usr/local/lib/libz.dylib
cd ..
cd ..

# libpng
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX10.13.sdk
if [ ! -d "libpng" ];
then
  git clone git://git.code.sf.net/p/libpng/code
  mv code libpng
fi
cd libpng
git pull
remake_dir _build_x86
cd _build_x86
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=i386\;x86_64 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6 -DPNG_EXECUTABLES=OFF -DPNG_SHARED=OFF -DPNG_TESTS=OFF -DZLIB_INCLUDE_DIR=/usr/local/include -DZLIB_LIBRARY_RELEASE=/usr/local/lib/libz.a -DPNG_BUILD_ZLIB=ON -DCMAKE_C_FLAGS="-arch x86_64"
make
cd ..
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX11.sdk
remake_dir _build_arm
cd _build_arm
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 -DPNG_EXECUTABLES=OFF -DPNG_SHARED=OFF -DPNG_TESTS=OFF -DPNG_ARM_NEON=off -DZLIB_INCLUDE_DIR=/usr/local/include -DZLIB_LIBRARY_RELEASE=/usr/local/lib/libz.a -DPNG_BUILD_ZLIB=ON
make
merge_libs ./ ../_build_x86 libpng16.a
sudo make install
cd ..
cd ..

# libjpeg
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX10.13.sdk
if [ ! -d "libjpeg-turbo" ];
then
  git clone https://github.com/libjpeg-turbo/libjpeg-turbo.git
fi
cd libjpeg-turbo
git pull
remake_dir _build_x86
cd _build_x86
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=i386\;x86_64 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6 -DENABLE_SHARED=OFF
make
cd ..
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX11.sdk
remake_dir _build_arm
cd _build_arm
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 -DENABLE_SHARED=OFF
make
merge_libs ./ ../_build_x86 libjpeg.a
sudo make install
cd ..
cd ..

# libwebp
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX10.13.sdk
if [ ! -d "libwebp" ];
then
  git clone https://chromium.googlesource.com/webm/libwebp
fi
cd libwebp
git pull
remake_dir _build_x86
cd _build_x86
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=i386\;x86_64 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6
make
cd ..
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX11.sdk
remake_dir _build_arm
cd _build_arm
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0
make
merge_libs ./ ../_build_x86 libwebp.a
merge_libs ./ ../_build_x86 libsharpyuv.a
sudo make install
cd ..
cd ..

# FreeType
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX10.13.sdk
if [ ! -d "freetype" ];
then
  git clone https://gitlab.freedesktop.org/freetype/freetype.git
fi
cd freetype
git pull
remake_dir _build_x86
cd _build_x86
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=i386\;x86_64 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6 -DFT_DISABLE_BROTLI=ON -DFT_DISABLE_BZIP2=ON -DFT_DISABLE_HARFBUZZ=ON -DFT_DISABLE_PNG=ON -DFT_DISABLE_ZLIB=OB
make
cd ..
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX11.sdk
remake_dir _build_arm
cd _build_arm
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 -DFT_DISABLE_BROTLI=ON -DFT_DISABLE_BZIP2=ON -DFT_DISABLE_HARFBUZZ=ON -DFT_DISABLE_PNG=ON -DFT_DISABLE_ZLIB=OB
make
merge_libs ./ ../_build_x86 libfreetype.a
sudo make install
cd ..
cd ..

# PhysFS
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX10.13.sdk
if [ ! -d "physfs" ];
then
  git clone https://github.com/icculus/physfs.git
fi
cd physfs
git pull
remake_dir _build_x86
cd _build_x86
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=i386\;x86_64 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6 -DPHYSFS_BUILD_SHARED=OFF -DPHYSFS_BUILD_DOCS=OFF
make
cd ..
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX11.sdk
remake_dir _build_arm
cd _build_arm
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 -DPHYSFS_BUILD_SHARED=OFF -DPHYSFS_BUILD_DOCS=OFF
make
merge_libs ./ ../_build_x86 libphysfs.a
sudo make install
cd ..
cd ..

# Allegro
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX10.13.sdk
if [ ! -d "allegro5" ];
then
  git clone https://github.com/liballeg/allegro5.git
fi
cd allegro5
git pull
remake_dir _build_x86
cd _build_x86
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=i386\;x86_64 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6 -DSHARED=OFF -DWANT_DEMO=OFF -DWANT_DOCS=OFF -DWANT_EXAMPLES=OFF -DWANT_NATIVE_IMAGE_LOADER=OFF -DWANT_TESTS=NO -DZLIB_INCLUDE_DIR=/usr/local/include -DZLIB_LIBRARY_RELEASE=/usr/local/lib/libz.a -DWANT_IMAGE_FREEIMAGE=NO
make
cd ..
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX11.sdk
remake_dir _build_arm
cd _build_arm
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 -DSHARED=OFF -DWANT_DEMO=OFF -DWANT_DOCS=OFF -DWANT_EXAMPLES=OFF -DWANT_NATIVE_IMAGE_LOADER=OFF -DWANT_TESTS=NO -DZLIB_INCLUDE_DIR=/usr/local/include -DZLIB_LIBRARY_RELEASE=/usr/local/lib/libz.a -DWANT_IMAGE_FREEIMAGE=NO
make
merge_libs lib ../_build_x86/lib liballegro-static.a
merge_libs lib ../_build_x86/lib liballegro_acodec-static.a
merge_libs lib ../_build_x86/lib liballegro_audio-static.a
merge_libs lib ../_build_x86/lib liballegro_color-static.a
merge_libs lib ../_build_x86/lib liballegro_dialog-static.a
merge_libs lib ../_build_x86/lib liballegro_font-static.a
merge_libs lib ../_build_x86/lib liballegro_image-static.a
merge_libs lib ../_build_x86/lib liballegro_main-static.a
merge_libs lib ../_build_x86/lib liballegro_memfile-static.a
merge_libs lib ../_build_x86/lib liballegro_physfs-static.a
merge_libs lib ../_build_x86/lib liballegro_primitives-static.a
merge_libs lib ../_build_x86/lib liballegro_ttf-static.a
sudo make install
cd ..
cd ..

# return to original location
cd $START_PATH
