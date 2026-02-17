#!/bin/bash

# set up variable defaults
BUILD_DUMB=1
BUILD_OGG=1
BUILD_VORBIS=1
BUILD_OPUS=1
BUILD_OPUSFILE=1
BUILD_FLAC=1
BUILD_MINIMP3=1
BUILD_ZLIB=1
BUILD_LIBPNG=1
BUILD_LIBJPEG=1
BUILD_LIBWEBP=1
BUILD_FREETYPE=1
BUILD_PHYSFS=1
BUILD_ALLEGRO=1

function remake_dir()
{
  rm -rf $1
  mkdir $1
}

function disable_all() {
  BUILD_DUMB=0
  BUILD_OGG=0
  BUILD_VORBIS=0
  BUILD_OPUS=0
  BUILD_OPUSFILE=0
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

# handle updating all dependencies for a single architecture
function update_dependencies_for()
{
  NDK_HOME=$HOME/Library/Android/sdk/ndk/29.0.14206865
  ANDROID_TOOLCHAIN_FILE=$NDK_HOME/build/cmake/android.toolchain.cmake
  INSTALL_PREFIX=$1/android/$2
  CMAKE_SETTINGS="-DCMAKE_TOOLCHAIN_FILE=$ANDROID_TOOLCHAIN_FILE -DANDROID_ABI=$2 -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX -DCMAKE_PREFIX_PATH=$1/android_$2"

  export ANDROID_HOME=$HOME/Library/Android/sdk
  export JAVA_HOME=$ANDROID_STUDIO_PATH/Contents/jbr/Contents/Home

  # DUMB
  if [ $BUILD_DUMB -eq 1 ]; then
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
  fi

  # OGG
  if [ $BUILD_OGG -eq 1 ]; then
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
  fi

  # Vorbis
  if [ $BUILD_VORBIS -eq 1 ]; then
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
  fi

  # Opus
  if [ $BUILD_OPUS -eq 1 ]; then
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
  fi

  # Opusfile
  if [ $BUILD_OPUSFILE -eq 1 ]; then
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
  fi

  # FLAC
  if [ $BUILD_FLAC -eq 1 ]; then
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
  fi

  # minimp3
  if [ $BUILD_MINIMP3 -eq 1 ]; then
    if [ ! -d "minimp3" ];
    then
      git clone https://github.com/lieff/minimp3.git
    fi
    cd minimp3
    git pull
    cp minimp3.h $INSTALL_PREFIX/include
    cp minimp3_ex.h $INSTALL_PREFIX/include
    cd ..
  fi

  # zlib
  if [ $BUILD_ZLIB -eq 1 ]; then
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
  fi

  # libpng
  if [ $BUILD_LIBPNG -eq 1 ]; then
    if [ ! -d "libpng" ];
    then
      git clone https://github.com/pnggroup/libpng.git
    fi
    cd libpng
    git pull
    remake_dir _build_android_$2
    cd _build_android_$2
    cmake .. $CMAKE_SETTINGS -DPNG_EXECUTABLES=OFF -DPNG_SHARED=OFF -DPNG_TESTS=OFF -DZLIB_INCLUDE_DIR=$INSTALL_PREFIX/include -DZLIB_LIBRARY_RELEASE=$INSTALL_PREFIX/lib/libz.a
    make
    make install
    cd ..
    cd ..
  fi

  # libjpeg
  if [ $BUILD_LIBJPEG -eq 1 ]; then
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
  fi

  # libwebp
  if [ $BUILD_LIBWEBP -eq 1 ]; then
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
  fi

  # FreeType
  if [ $BUILD_FREETYPE -eq 1 ]; then
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
  fi

  # PhysFS
  if [ $BUILD_PHYSFS -eq 1 ]; then
    if [ ! -d "physfs" ];
    then
      git clone https://github.com/icculus/physfs.git
    fi
    cd physfs
    git pull
    remake_dir _build_android_$2
    cd _build_android_$2
    cmake .. $CMAKE_SETTINGS -DPHYSFS_BUILD_SHARED=OFF -DPHYSFS_BUILD_DOCS=OFF -DPHYSFS_ARCHIVE_7Z=OFF -DPHYSFS_ARCHIVE_GRP=OFF -DPHYSFS_ARCHIVE_HOG=OFF -DPHYSFS_ARCHIVE_ISO9660=OFF -DPHYSFS_ARCHIVE_MVL=OFF -DPHYSFS_ARCHIVE_QPAK=OFF -DPHYSFS_ARCHIVE_SLB=OFF -DPHYSFS_ARCHIVE_VDF=OFF -DPHYSFS_ARCHIVE_WAD=OFF -DCMAKE_C_FLAGS="-DLACKING_PROTOENT"
    make
    make install
    cd ..
    cd ..
  fi

  # Allegro
  if [ $BUILD_ALLEGRO -eq 1 ]; then
    if [ ! -d "allegro5" ];
    then
      git clone https://github.com/liballeg/allegro5.git
    fi
    cd allegro5
    git pull
    android/gradle_project/gradlew --stop
    remake_dir _build_android_$2
    cd _build_android_$2
    ln -s $INSTALL_PREFIX deps
    cmake .. $CMAKE_SETTINGS -DWANT_DEMO=OFF -DWANT_DOCS=OFF -DWANT_EXAMPLES=OFF -DWANT_NATIVE_IMAGE_LOADER=OFF -DWANT_TESTS=NO -DZLIB_INCLUDE_DIR=$INSTALL_PREFIX/include -DZLIB_LIBRARY_RELEASE=$INSTALL_PREFIX/lib/libz.a -DWANT_IMAGE_FREEIMAGE=NO -DWANT_OPENAL=NO -DANDROID_PLATFORM=android-33 -DANDROID_TARGET=android-34
    make
#    sed -i '' "s|3.2.0|7.4.2|" android/gradle_project/build.gradle
#    sed -i '' "s|gradle-5.0-bin.zip|gradle-8.5-bin.zip|" android/gradle_project/gradle/wrapper/gradle-wrapper.properties
    make
    make install
    cp lib/allegro-release.aar $INSTALL_PREFIX/lib
    cd ..
    android/gradle_project/gradlew --stop
    cd ..
  fi

}

if [ "$#" -le 0 ]; then
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

update_dependencies_for $1 "armeabi-v7a";
update_dependencies_for $1 "arm64-v8a";
update_dependencies_for $1 "x86";
update_dependencies_for $1 "x86_64";
