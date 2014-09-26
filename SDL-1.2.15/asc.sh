#!/bin/bash

NDKROOT="/home/nathan/android/adt/ndk"

NDKTOOLPATH="toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86"

NDKSYSROOT="$NDKROOT/platforms/android-8/arch-arm"

SDLPATH="/home/nathan/SDL-1.2.15"

CFLAGS="--sysroot=$NDKSYSROOT \
    -mandroid -march=armv7-a -mfloat-abi=softfp -mfpu=neon \
    -I$SDLPATH/Inst/include \
	-lz -llog -lc -lm -L$SDLPATH/Inst/lib -l:libSDL.a "

LDFLAGS="--sysroot=$NDKSYSROOT \
	-lz -llog -lc -lm -L$SDLPATH/Inst/lib -l:libSDL.a "
	
CFLAGS="--sysroot=$NDKSYSROOT \
    -mandroid -march=armv7-a -mfloat-abi=softfp -mfpu=neon \
    -DNO_MNTENT -UHAVE_ICONV -DNO_MALLINFO=1 "

LDFLAGS="-llog -lc -llog -lm -ldl "

PATH="$NDKROOT:$NDKROOT/bin:$SDLPATH:$PATH"

#clean the slate ready
make clean

chmod +x ./Inst/bin/sdl-config

PATH="$PATH" \
CFLAGS="$CFLAGS" \
LDFLAGS="$LDFLAGS" \
./configure \
--host=arm-linux-androideabi --enable-static=yes \
--prefix=/home/nathan/android/SDL-1.2.15/Inst \
--enable-video-android=yes --enable-androidaudio=yes \
--enable-video-x11=no --enable-pth=no --enable-pulseaudio=no \
--enable-pthreads=yes --enable-pthread-sem=yes \
--enable-audio=yes --enable-video=yes --enable-events=yes \
--enable-joystick=yes --enable-cdrom=yes --enable-threads=yes \
--enable-timers=yes \
--enable-pth=no --enable-nasm=no --enable-video-x11=no \
--enable-pulseaudio=no --enable-oss=no --enable-directx=no \
--enable-pulseaudio-shared=no --enable-video-dummy=yes \
--enable-sdl-dlopen=no --enable-video-fbcon=yes --enable-assembly=no \
--enable-esd=no --enable-arts=no --enable-diskaudio=no \
--enable-mintaudio=no 


#--enable-pthreads=yes --enable-pthread-sem=yes \
#--enable-audio=yes --enable-video=yes --enable-events=yes --enable-joystick=yes \
#--enable-cdrom=yes --enable-threads=yes --enable-timers=yes

read

PATH="$PATH" \
CFLAGS="$CFLAGS" \
LDFLAGS="$LDFLAGS" \
CC=arm-linux-androideabi-gcc \
make

if [ -e build/.libs/libSDL.a ]; then

	chmod +x ./Inst/bin/sdl-config
	make install DESTDIR=`pwd`/Inst
	cp -R Inst/home/nathan/android/SDL-1.2.15/Inst/* Inst/
	
fi



#
#	--sysroot=$NDKROOT/platforms/android-5/arch-arm 
#	-iprefix=$NDKROOT/platforms/android-5/arch-arm 
#CPPFLAGS="$CFLAGS"
#CXXFLAGS="$CFLAGS"


#CXXFLAGS="$CXFLAGS" \
#LDFLAGS="$LDFLAGS" \


#--enable-oss=no \
#--enable-pth=no --enable-pthreads=yes --enable-pthread-sem=yes \
#--enable-sdl-dlopen=no --enable-alsa=yes --enable-static=yes \
#--enable-pulseaudio-shared=no \
#--enable-audio=yes --enable-video=yes --enable-events=yes --enable-joystick=yes \
#--enable-cdrom=yes  --enable-threads=yes --enable-timers=yes \
#--enable-video-dummy=no --enable-directx=no

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
