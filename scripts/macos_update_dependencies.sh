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
X86_SDK=MacOSX10.13.sdk
ARM_SDK=MacOSX11.sdk

mkdir -p $1
cd $1

# DUMB
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$X86_SDK
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
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$ARM_SDK
remake_dir _build_arm
cd _build_arm
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 -DBUILD_ALLEGRO4=OFF -DBUILD_EXAMPLES=OFF
make
merge_libs ./ ../_build_x86 libdumb.a
sudo make install
cd ..
cd ..

# OpenMPT
#SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$X86_SDK
#if [ ! -d "openmpt" ];
#then
#  git clone https://github.com/OpenMPT/openmpt.git
#fi
#cd openmpt
#git pull
#make clean
#CFLAGS="-arch i386 -mmacos-version-min=10.6 -isysroot $SDK_PATH" CXXFLAGS="-arch i386 -mmacos-version-min=10.6 -isysroot $SDK_PATH -std=c++11" make
#mv bin/libopenmpt.a libopenmpt_i386.a
#make clean
#CFLAGS="-arch x86_64 -mmacos-version-min=10.6 -isysroot $SDK_PATH" CXXFLAGS="-arch x86_64 -mmacos-version-min=10.6 -isysroot $SDK_PATH" make
#mv bin/libopenmpt.a libopenmpt_x86_64.a
#SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$ARM_SDK
#make clean
#CFLAGS="-arch arm64 -mmacos-version-min=11.0 -isysroot $SDK_PATH" CXXFLAGS="-arch arm64 -mmacos-version-min=11.0 -isysroot $SDK_PATH" make
#lipo -create lipopenmpt_i386.a libopenmpt_x86_64.a bin/libopenmpt.a -output bin/libopenmpt.a
#sudo make install
#sudo rm /usr/local/lib/libopenmpt.so
#cd ..

# OGG
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$X86_SDK
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
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$ARM_SDK
remake_dir _build_arm
cd _build_arm
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0
make
merge_libs ./ ../_build_x86 libogg.a
sudo make install
cd ..
cd ..

# Vorbis
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$X86_SDK
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
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$ARM_SDK
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
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$X86_SDK
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
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$ARM_SDK
remake_dir _build_arm
cd _build_arm
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0
make
merge_libs ./ ../_build_x86 libopus.a
sudo make install
cd ..
cd ..

# Opusfile
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$X86_SDK
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
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$ARM_SDK
remake_dir _build_arm
cd _build_arm
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 -DOP_DISABLE_DOCS=ON -DOP_DISABLE_EXAMPLES=ON -DOP_DISABLE_HTTP=ON
make
merge_libs ./ ../_build_x86 libopusfile.a
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
./configure CFLAGS="-arch i386 -arch x86_64 -isysroot /Library/Developer/CommandLineTools/SDKs/$X86_SDK -mmacos-version-min=10.6" --enable-shared=no --enable-static=yes
make clean
make
mv lib/.libs/libiconv.a libiconv.a
./configure CFLAGS="-arch arm64 -isysroot /Library/Developer/CommandLineTools/SDKs/$ARM_SDK -mmacos-version-min=11.0" --enable-shared=no --enable-static=yes --host=`uname -m`-apple-darwin
make clean
make
merge_libs lib/.libs . libiconv.a
sudo make install
cd ..
cd ..

# FLAC
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$X86_SDK
if [ ! -d "flac" ];
then
  git clone https://github.com/xiph/flac.git
fi
cd flac
git pull
remake_dir _build_x86
cd _build_x86
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=i386\;x86_64 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6 -DBUILD_DOCS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_PROGRAMS=OFF -DBUILD_TESTING=OFF -DINSTALL_MANPAGES=OFF -DIconv_INCLUDE_DIR=/usr/local/include -DIconv_LIBRARY=/usr/local/lib/libiconv.a
make
cd ..
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$ARM_SDK
remake_dir _build_arm
cd _build_arm
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 -DBUILD_DOCS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_PROGRAMS=OFF -DBUILD_TESTING=OFF -DINSTALL_MANPAGES=OFF -DIconv_INCLUDE_DIR=/usr/local/include -DIconv_LIBRARY=/usr/local/lib/libiconv.a
make
merge_libs src/libFLAC ../_build_x86/src/libFLAC libFLAC.a
merge_libs src/libFLAC++ ../_build_x86/src/libFLAC++ libFLAC++.a
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
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$X86_SDK
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
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$ARM_SDK
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
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$X86_SDK
if [ ! -d "libpng" ];
then
  git clone https://github.com/pnggroup/libpng.git
fi
cd libpng
git pull
remake_dir _build_i386
cd _build_i386
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=i386 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6 -DPNG_TOOLS=OFF -DPNG_SHARED=OFF -DPNG_FRAMEWORK=OFF -DPNG_TESTS=OFF -DZLIB_ROOT=/usr/local -DCMAKE_C_FLAGS=-D__i386__
make
cd ..
remake_dir _build_x86_64
cd _build_x86_64
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=x86_64 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6 -DPNG_TOOLS=OFF -DPNG_SHARED=OFF -DPNG_FRAMEWORK=OFF -DPNG_TESTS=OFF -DZLIB_ROOT=/usr/local -DCMAKE_C_FLAGS=-D__x86_64__
make
cd ..
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$ARM_SDK
remake_dir _build_arm
cd _build_arm
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 -DPNG_TOOLS=OFF -DPNG_SHARED=OFF -DPNG_FRAMEWORK=OFF -DPNG_TESTS=OFF -DPNG_ARM_NEON=off -DZLIB_ROOT=/usr/local
make
merge_libs ./ ../_build_i386 libpng16.a
merge_libs ./ ../_build_x86_64 libpng16.a
sudo make install
cd ..
cd ..

# libjpeg
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$X86_SDK
if [ ! -d "libjpeg-turbo" ];
then
  git clone https://github.com/libjpeg-turbo/libjpeg-turbo.git
fi
cd libjpeg-turbo
git pull
remake_dir _build_i386
cd _build_i386
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=i386 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6 -DENABLE_SHARED=OFF
make
cd ..
remake_dir _build_x86_64
cd _build_x86_64
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=x86_64 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6 -DENABLE_SHARED=OFF
make
cd ..
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$ARM_SDK
remake_dir _build_arm
cd _build_arm
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 -DENABLE_SHARED=OFF
make
merge_libs ./ ../_build_i386 libjpeg.a
merge_libs ./ ../_build_x86_64 libjpeg.a
sudo make install
cd ..
cd ..

# libwebp
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$X86_SDK
if [ ! -d "libwebp" ];
then
  git clone https://chromium.googlesource.com/webm/libwebp
fi
cd libwebp
git pull
remake_dir _build_x86
cd _build_x86
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=i386\;x86_64 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6 -DZLIB_INCLUDE_DIR=/usr/local/include -DZLIB_LIBRARY_RELEASE=/usr/local/lib/libz.a -DWEBP_BUILD_CWEBP=OFF -DWEBP_BUILD_DWEBP=OFF
make
cd ..
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$ARM_SDK
remake_dir _build_arm
cd _build_arm
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 -DZLIB_INCLUDE_DIR=/usr/local/include -DZLIB_LIBRARY_RELEASE=/usr/local/lib/libz.a -DWEBP_BUILD_CWEBP=OFF -DWEBP_BUILD_DWEBP=OFF
make
merge_libs ./ ../_build_x86 libwebp.a
merge_libs ./ ../_build_x86 libsharpyuv.a
sudo make install
cd ..
cd ..

# FreeType
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$X86_SDK
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
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$ARM_SDK
remake_dir _build_arm
cd _build_arm
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 -DFT_DISABLE_BROTLI=ON -DFT_DISABLE_BZIP2=ON -DFT_DISABLE_HARFBUZZ=ON -DFT_DISABLE_PNG=ON -DFT_DISABLE_ZLIB=OB
make
merge_libs ./ ../_build_x86 libfreetype.a
sudo make install
cd ..
cd ..

# PhysFS
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$X86_SDK
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
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$ARM_SDK
remake_dir _build_arm
cd _build_arm
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 -DPHYSFS_BUILD_SHARED=OFF -DPHYSFS_BUILD_DOCS=OFF
make
merge_libs ./ ../_build_x86 libphysfs.a
sudo make install
cd ..
cd ..

# Allegro
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$X86_SDK
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
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/$ARM_SDK
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
