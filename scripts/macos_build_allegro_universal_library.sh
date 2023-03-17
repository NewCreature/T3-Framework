#!/bin/bash

function merge_libs() {
  lipo -create $1/$3 $2/$3 -output $1/$3
}

if [ "$#" -ne 1 ]; then
    echo "Usage: macos_build_allegro_universal_library <path_to_allegro>"
    exit 1
fi

cd $1

# build x86 libraries
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX10.13.sdk
mkdir _build_x86
cd _build_x86
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=i386\;x86_64 -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6 -DFREETYPE_PNG=ON -DFREETYPE_ZLIB=ON -DSHARED=OFF -DWANT_DEMO=OFF -DWANT_DOCS=OFF -DWANT_EXAMPLES=OFF -DWANT_NATIVE_IMAGE_LOADER=OFF -DWANT_TESTS=NO -DZLIB_INCLUDE_DIR=/usr/local/include -DZLIB_LIBRARY_RELEASE=/usr/local/lib/libz.a -DWANT_IMAGE_FREEIMAGE=NO
make
cd ..

# build ARM libraries
SDK_PATH=/Library/Developer/CommandLineTools/SDKs/MacOSX11.1.sdk
mkdir _build_arm
cd _build_arm
cmake .. -DCMAKE_OSX_SYSROOT=$SDK_PATH -DCMAKE_OSX_ARCHITECTURES=arm64 -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0 -DFREETYPE_PNG=ON -DFREETYPE_ZLIB=ON -DSHARED=OFF -DWANT_DEMO=OFF -DWANT_DOCS=OFF -DWANT_EXAMPLES=OFF -DWANT_NATIVE_IMAGE_LOADER=OFF -DWANT_TESTS=NO -DZLIB_INCLUDE_DIR=/usr/local/include -DZLIB_LIBRARY_RELEASE=/usr/local/lib/libz.a -DWANT_IMAGE_FREEIMAGE=NO
make

# combine libraries
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

# install libraries
sudo make install

# clean up
cd ..
rm -rf _build_x86
rm -rf _build_arm
