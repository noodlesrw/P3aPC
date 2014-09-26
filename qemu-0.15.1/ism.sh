#!/bin/bash

XPATH=""

CFLAGS="-g "

CPPFLAGS="$CFLAGS"
CXXFLAGS="$CFLAGS"

LDFLAGS="-g -lz -lc -lm"

PATH="$XPATH:$PATH" \
CFLAGS="$CFLAGS" \
LDFLAGS="-Bstatic -l:libSDL.a $LDFLAGS" \
make

if [ -e i386-softmmu/qemu ]; then
strip i386-softmmu/qemu
cp i386-softmmu/qemu i386-softmmu/qemu-x86
fi


#toolpath=/home/nathan/android-ndk-r7/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin
#toolpath+=:/home/nathan/android-ndk-r7/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/lib/gcc/arm-linux-androideabi/4.4.3
#toolpath+=:/home/nathan/android-ndk-r7/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86

#xprefix=arm-linux-androideabi-

#usrpath=/home/nathan/android-ndk-r7/platforms/android-5/arch-arm/usr
#crtpath=/home/nathan/android-ndk-r7/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/lib/gcc/arm-linux-androideabi/4.4.3

#usrpath=/home/nathan/android/gcc4x/arm-linux-elf
#crtpath=/home/nathan/android/gcc4x/lib/gcc/arm-linux-elf/4.5.0

#PATH=$toolpath:$PATH \
#LDFLAGS="$LDFLAGS \
#/home/nathan/android-ndk-r7/platforms/android-5/arch-arm/usr/lib/crtbegin_dynamic.o \
#-L/home/nathan/android-ndk-r7/platforms/android-5/arch-arm/usr/lib -lc -nostdlib " \
#CFLAGS="$CFLAGS -nostdlib" \
#make
