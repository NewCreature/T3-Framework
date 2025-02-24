#!/bin/bash

# set up variable defaults
START_PATH=$(pwd)
X86_SDK=MacOSX10.13.sdk
ARM_SDK=MacOSX11.sdk
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

function merge_libs() {
  lipo -create $1/$3 $2/$3 -output $1/$3
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
  echo "Usage: macos_update_dependencies <path> <arguments>"
  exit 1
fi

# check arguments
for arg in "$@";
do
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

# DUMB
if [ $BUILD_DUMB -eq 1 ]; then
  echo "Updating DUMB..."
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
fi

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
if [ $BUILD_OGG -eq 1 ]; then
  echo "Updating libogg..."
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
fi

# Vorbis
if [ $BUILD_VORBIS -eq 1 ]; then
  echo "Updating libvorbis..."
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
fi

# Opus
if [ $BUILD_OPUS -eq 1 ]; then
  echo "Updating Opus..."
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
fi

# Opusfile
if [ $BUILD_OPUSFILE -eq 1 ]; then
  echo "Updating Opusfile..."
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
fi

# libiconv (needed by FLAC)
if [ $BUILD_LIBICONV -eq 1 ]; then
  echo "Updating libiconv..."
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
fi

# FLAC
if [ $BUILD_FLAC -eq 1 ]; then
  echo "Updating FLAC..."
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
fi

# libpng
if [ $BUILD_LIBPNG -eq 1 ]; then
  echo "Updating libpng..."
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
fi

# libjpeg
if [ $BUILD_LIBJPEG -eq 1 ]; then
  echo "Updating libjpeg..."
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
fi

# libwebp
if [ $BUILD_LIBWEBP -eq 1 ]; then
  echo "Updating libwebp..."
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
fi

# FreeType
if [ $BUILD_FREETYPE -eq 1 ]; then
  echo "Updating libfreetype..."
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
fi

# PhysFS
if [ $BUILD_PHYSFS -eq 1 ]; then
  echo "Updating PhysFS..."
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
fi

# Allegro
if [ $BUILD_ALLEGRO -eq 1 ]; then
  echo "Updating Allegro..."
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
fi

# return to original location
cd $START_PATH
