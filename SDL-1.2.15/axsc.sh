#!/bin/bash

#clean the slate ready
make clean

export NDKROOT=/home/nathan/android-ndk-r7

#NDKTOOLPATH=toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin
NDKTOOLPATH=toolchains/x86-4.4.3/prebuilt/linux-x86

chmod +x ./Inst/bin/sdl-config

#baArch="arch-arm"
#baHost="arm-linux-androideabi"
baArch="arch-x86"
baHost="i686-android-linux"

CPPFLAGS="--sysroot=$NDKROOT/platforms/android-5/$baArch " \
   CFLAGS="--sysroot=$NDKROOT/platforms/android-5/$baArch \
   -mandroid -DNO_MNTENT " \
   CXXFLAGS="--sysroot=$NDKROOT/platforms/android-5/$baArch " \
   LDFLAGS="--sysroot=$NDKROOT/platforms/android-5/$baArch \
   -mandroid \
   -L/home/nathan/android-ndk-r7/platforms/android-5/$baArch/usr/lib " \
CC=$NDKROOT/$NDKTOOLPATH/$baHost-gcc \
./configure \
--build=$baHost --host=i686-pc-linux-gnu \
--prefix=/home/nathan/android/SDL-1.2.15/Inst \
--enable-nasm=no --enable-oss=no --enable-video-x11=no \
--enable-pth=no --enable-pthreads --enable-pthread-sem \
--enable-sdl-dlopen=no --enable-alsa --enable-static=yes \
--enable-pulseaudio=no --enable-pulseaudio-shared=no \
--enable-audio=yes --enable-video=yes --enable-events=yes --enable-joystick=yes \
--enable-cdrom=yes  --enable-threads=yes --enable-timers=yes \
--enable-video-dummy=no --enable-directx=no

