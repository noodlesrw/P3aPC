#!/bin/bash

NDKROOT=/home/nathan/android-ndk-r7

NDKTOOLPATH=toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin
#/home/nathan/android-ndk-r7/platforms/android-5/arch-arm/usr/include

SDLPATH="/home/nathan/android/SDL-1.2.15"

CFLAGS="-g -mandroid -Wl,--export-dynamic,--discard-none -I$SDLPATH/Inst/include \
   --sysroot=$NDKROOT/platforms/android-5/arch-arm \
   -I$NDKROOT/platforms/android-5/arch-arm/usr/include \
   -L$NDKROOT/platforms/android-5/arch-arm/usr/lib "
CPPFLAGS="$CFLAGS"
CXXFLAGS="$CFLAGS"
LDFLAGS="-mandroid -I$SDLPATH/Inst/lib \
   --sysroot=$NDKROOT/platforms/android-5/arch-arm \
   -L$NDKROOT/platforms/android-5/arch-arm/usr/lib \
    -lz -lc -lm "

PATH="$SDLPATH:$SDLPATH/Inst/bin:$NDKROOT/$NDKTOOLPATH:$PATH" \
CFLAGS="$CFLAGS" \
LDFLAGS="$LDFLAGS" \
make $1
