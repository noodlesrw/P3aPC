#!/bin/bash

NDKROOT="/home/nathan/android/adt/ndk"

NDKTOOLPATH="toolchains/arm-linux-androideabi-4.6/prebuilt/linux-x86"

NDKSYSROOT="$NDKROOT/platforms/android-8/arch-arm/"

SDLPATH="/home/nathan/SDL-1.2.15"

CFLAGS=" -DNO_MNTENT -UHAVE_ICONV -DNO_MALLINFO=1 \
	--sysroot $NDKSYSROOT \
    -isystem $NDKSYSROOT \
	-I$NDKSYSROOT/usr/include \
    -mandroid \
    -L$NDKSYSROOT/usr/lib "
CPPFLAGS="$CFLAGS"
CXXFLAGS="$CFLAGS"
LDFLAGS="--sysroot=$NDKSYSROOT  \
	-L$NDKSYSROOT/usr/lib -llog -lc -llog -lm -ldl "

PATH="$NDKROOT:$NDKROOT/$NDKTOOLPATH/bin:$NDKROOT/$NDKTOOLPATH:$PATH"

#clean the slate ready
make clean

chmod +x ./Inst/bin/sdl-config

PATH="$PATH" \
./configure \
CFLAGS="$CFLAGS" \
CXXFLAGS="$CXFLAGS" \
LDFLAGS="$LDFLAGS" \
--host=arm-linux-androideabi --enable-static \
--prefix=/home/nathan/SDL-1.2.15/Inst \
--enable-video-android=yes --enable-androidaudio=yes \
--enable-nasm=no --enable-oss=no --enable-video-x11=no --enable-pth=no \
--enable-pthreads=yes --enable-pthread-sem=yes \
--enable-sdl-dlopen=yes --enable-alsa=yes --enable-static=yes \
--enable-pulseaudio=no --enable-pulseaudio-shared=no \
--enable-audio=yes --enable-video=yes --enable-events=yes --enable-joystick=yes \
--enable-cdrom=yes  --enable-threads=yes --enable-timers=yes \
--enable-video-dummy=no --enable-directx=no

#--enable-pth=no 
#--enable-pthread-sem 

# Build, and install in `pwd`/Inst
#
#if [ ! -e "Inst" ]; then
#	mkdir Inst
#fi
#make


#--prefix=/home/nathan/android/gcc4x/arm-linux-elf/lib \
#--exec-prefix=/home/nathan/android/gcc4x/arm-linux-elf/lib \
#/home/nathan/android-ndk-r7/platforms/android-5/arch-arm/usr/lib/crtbegin_so.o
#-I/home/nathan/android-ndk-r7/platforms/android-5/arch-arm/usr/include \
