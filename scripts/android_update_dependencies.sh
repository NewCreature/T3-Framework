#!/bin/bash

function remake_dir()
{
  rm -rf $1
  mkdir $1
}

# handle updating all dependencies for a single architecture
function update_dependencies_for()
{
  NDK_HOME=$HOME/Library/Android/sdk/ndk/25.2.9519653
  ANDROID_TOOLCHAIN_FILE=$NDK_HOME/build/cmake/android.toolchain.cmake
  INSTALL_PREFIX=$1/android/$2
  CMAKE_SETTINGS="-DCMAKE_TOOLCHAIN_FILE=$ANDROID_TOOLCHAIN_FILE -DANDROID_ABI=$2 -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX -DCMAKE_PREFIX_PATH=$1/android_$2"

  export ANDROID_HOME=$HOME/Library/Android
  export JAVA_HOME=$ANDROID_STUDIO_PATH/Contents/jbr/Contents/Home

  # DUMB
  if [ ! -d "dumb" ];
  then
    git clone https://github.com/kode54/dumb.git
  fi
  cd dumb
  git pull
  remake_dir _build_android_$2
  cd _build_android_$2
  cmake .. $CMAKE_SETTINGS -DBUILD_ALLEGRO4=OFF -DBUILD_EXAMPLES=OFF
  make
  make install
  cd ..
  cd ..

  # OGG
  if [ ! -d "ogg" ];
  then
    git clone https://github.com/xiph/ogg.git
  fi
  cd ogg
  git pull
  remake_dir _build_android_$2
  cd _build_android_$2
  cmake .. $CMAKE_SETTINGS
  make
  make install
  cd ..
  cd ..

  # Vorbis
  if [ ! -d "vorbis" ];
  then
    git clone https://github.com/xiph/vorbis.git
  fi
  cd vorbis
  git pull
  remake_dir _build_android_$2
  cd _build_android_$2
  cmake .. $CMAKE_SETTINGS -DBUILD_TESTING=OFF -DOGG_INCLUDE_DIR=$INSTALL_PREFIX/include -DOGG_LIBRARY=$INSTALL_PREFIX/lib/libogg.a
  make
  make install
  cd ..
  cd ..

  # Opus
  if [ ! -d "opus" ];
  then
    git clone https://github.com/xiph/opus.git
  fi
  cd opus
  git pull
  remake_dir _build_android_$2
  cd _build_android_$2
  cmake .. $CMAKE_SETTINGS
  make
  make install
  cd ..
  cd ..

  # Opusfile
  if [ ! -d "opusfile" ];
  then
    git clone https://github.com/xiph/opusfile.git
  fi
  cd opusfile
  git pull
  remake_dir _build_android_$2
  cd _build_android_$2
  cmake .. $CMAKE_SETTINGS -DOP_DISABLE_DOCS=ON -DOP_DISABLE_EXAMPLES=ON -DOP_DISABLE_HTTP=ON
  make
  make install
  cd ..
  cd ..

  # FLAC
  if [ ! -d "flac" ];
  then
    git clone https://github.com/xiph/flac.git
  fi
  cd flac
  git pull
  remake_dir _build_android_$2
  cd _build_android_$2
  cmake .. $CMAKE_SETTINGS -DBUILD_DOCS=OFF -DBUILD_EXAMPLES=OFF -DBUILD_PROGRAMS=OFF -DBUILD_TESTING=OFF -DINSTALL_MANPAGES=OFF -DOGG_INCLUDE_DIR=$INSTALL_PREFIX/include -DOGG_LIBRARY=$INSTALL_PREFIX/lib/libogg.a
  make
  make install
  cd ..
  cd ..

  # minimp3
  if [ ! -d "minimp3" ];
  then
    git clone https://github.com/lieff/minimp3.git
  fi
  cd minimp3
  git pull
  cp minimp3.h $INSTALL_PREFIX/include
  cp minimp3_ex.h $INSTALL_PREFIX/include
  cd ..

  # zlib
  if [ ! -d "zlib" ];
  then
    git clone https://github.com/madler/zlib.git
  fi
  cd zlib
  git pull
  remake_dir _build_android_$2
  cd _build_android_$2
  cmake .. $CMAKE_SETTINGS
  make
  make install
  rm -f $INSTALL_PREFIX/lib/libz.so*
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
  remake_dir _build_android_$2
  cd _build_android_$2
  cmake .. $CMAKE_SETTINGS -DPNG_EXECUTABLES=OFF -DPNG_SHARED=OFF -DPNG_TESTS=OFF -DZLIB_INCLUDE_DIR=$INSTALL_PREFIX/include -DZLIB_LIBRARY_RELEASE=$INSTALL_PREFIX/lib/libz.a -DPNG_BUILD_ZLIB=ON
  make
  make install
  cd ..
  cd ..

  # libjpeg
  if [ ! -d "libjpeg-turbo" ];
  then
    git clone https://github.com/libjpeg-turbo/libjpeg-turbo.git
  fi
  cd libjpeg-turbo
  git pull
  remake_dir _build_android_$2
  cd _build_android_$2
  cmake .. $CMAKE_SETTINGS -DENABLE_SHARED=OFF
  make
  make install
  cd ..
  cd ..

  # libwebp
  if [ ! -d "libwebp" ];
  then
    git clone https://chromium.googlesource.com/webm/libwebp
  fi
  cd libwebp
  git pull
  remake_dir _build_android_$2
  cd _build_android_$2
  cmake .. $CMAKE_SETTINGS
  make
  make install
  cd ..
  cd ..

  # FreeType
  if [ ! -d "freetype" ];
  then
    git clone https://gitlab.freedesktop.org/freetype/freetype.git
  fi
  cd freetype
  git pull
  remake_dir _build_android_$2
  cd _build_android_$2
  cmake .. $CMAKE_SETTINGS -DFT_DISABLE_BROTLI=ON -DFT_DISABLE_BZIP2=ON -DFT_DISABLE_HARFBUZZ=ON -DFT_DISABLE_PNG=ON -DFT_DISABLE_ZLIB=ON
  make
  make install
  cd ..
  cd ..

  # PhysFS
  if [ ! -d "physfs" ];
  then
    git clone https://github.com/icculus/physfs.git
  fi
  cd physfs
  git pull
  remake_dir _build_android_$2
  cd _build_android_$2
  cmake .. $CMAKE_SETTINGS -DPHYSFS_BUILD_SHARED=OFF -DPHYSFS_BUILD_DOCS=OFF -DPHYSFS_ARCHIVE_7Z=OFF -DPHYSFS_ARCHIVE_GRP=OFF -DPHYSFS_ARCHIVE_HOG=OFF -DPHYSFS_ARCHIVE_ISO9660=OFF -DPHYSFS_ARCHIVE_MVL=OFF -DPHYSFS_ARCHIVE_QPAK=OFF -DPHYSFS_ARCHIVE_SLB=OFF -DPHYSFS_ARCHIVE_VDF=OFF -DPHYSFS_ARCHIVE_WAD=OFF
  make
  make install
  cd ..
  cd ..

  # Allegro
  if [ ! -d "allegro5" ];
  then
    git clone https://github.com/liballeg/allegro5.git
  fi
  cd allegro5
  git pull
  remake_dir _build_android_$2
  cd _build_android_$2
  ln -s $INSTALL_PREFIX deps
  cmake .. $CMAKE_SETTINGS -DWANT_DEMO=OFF -DWANT_DOCS=OFF -DWANT_EXAMPLES=OFF -DWANT_NATIVE_IMAGE_LOADER=OFF -DWANT_TESTS=NO -DZLIB_INCLUDE_DIR=$INSTALL_PREFIX/include -DZLIB_LIBRARY_RELEASE=$INSTALL_PREFIX/lib/libz.a -DWANT_IMAGE_FREEIMAGE=NO -DWANT_OPENAL=NO -DANDROID_TARGET=android-26
  make
	sed -i '' "s|3.2.0|7.4.2|" android/gradle_project/build.gradle
	sed -i '' "s|gradle-5.0-bin.zip|gradle-8.1.1-bin.zip|" android/gradle_project/gradle/wrapper/gradle-wrapper.properties
  make
  make install
  cp lib/allegro-release.aar $INSTALL_PREFIX/lib
  cd ..
  cd ..

}

if [ "$#" -ne 1 ]; then
  echo "Usage: android_update_dependencies.sh <path>"
  exit 1
fi

START_PATH=$(pwd)
if [ -d "/Applications/Android Studio.app" ];
then
  export ANDROID_STUDIO_PATH="/Applications/Android Studio.app"
else
  echo "Android Studio not found in standard location, searching..."
  export ANDROID_STUDIO_PATH=`find -L /Applications -name "Android Studio.app"`
fi

mkdir -p $1
cd $1

update_dependencies_for $1 "armeabi-v7a";
update_dependencies_for $1 "arm64-v8a";
update_dependencies_for $1 "x86";
update_dependencies_for $1 "x86_64";
