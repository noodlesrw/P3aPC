#!/bin/bash

NDKROOT="/home/nathan/android-ndk-r7"

NDKTOOLPATH="toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86"
#/home/nathan/android-ndk-r7/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin
SDLPATH="/home/nathan/android/SDL-1.2.15"
#/home/nathan/android-ndk-r7/platforms/android-5/arch-arm/usr/lib
#	-iprefix=$NDKROOT/platforms/android-5/arch-arm/ \

CFLAGS="--sysroot=$NDKROOT/platforms/android-5/arch-arm/ \
	-iprefix=$NDKROOT/platforms/android-5/arch-arm/ \
	-mandroid \
    -I$SDLPATH/Inst/include \
	-L$SDLPATH/Inst/lib -l:libSDL.a -l:libSDLmain.a -llog \
	-Wl,-E,-q,--discard-none,-z,defs,-pie "
#	-lz -llog -lc -lm \


#    -I$NDKROOT/platforms/android-5/arch-arm/usr/include \
#    -L$NDKROOT/platforms/android-5/arch-arm/usr/lib "
#    -L/usr/lib -I/usr/include "
#CPPFLAGS="$CFLAGS" 

CFLAGS="$CFLAGS" \
PATH="$SDLPATH:$SDLPATH/Inst/bin:$NDKROOT:$NDKROOT/$NDKTOOLPATH/bin:$NDKROOT/$NDKTOOLPATH:$PATH" \
./configure --host=arm-linux-androideabi


CFLAGS="$CFLAGS" \
PATH="$SDLPATH:$SDLPATH/Inst/bin:$NDKROOT:$NDKROOT/$NDKTOOLPATH/bin:$NDKROOT/$NDKTOOLPATH:$PATH" \
CC=arm-linux-androideabi-gcc \
make

#if [ -e sdl-pong ]; then
#CFLAGS="$CFLAGS" \
#PATH="$SDLPATH:$SDLPATH/Inst/bin:$NDKROOT:$NDKROOT/$NDKTOOLPATH/bin:$NDKROOT/$NDKTOOLPATH:$PATH" \
#arm-linux-androideabi-strip sdl-pong
#cp sdl-pong /home/nathan/android/SDLapp/libs/armeabi/libnativeapp.so
#fi

#-D_FORTIFY_SOURCE=2 -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
#-Wredundant-decls -Wall -Wundef -Wwrite-strings \
#-fno-strict-aliasing  -Wendif-labels -Wmissing-include-dirs \
#-Wempty-body -Wformat-security -Wformat-y2k -Winit-self \
#-Wignored-qualifiers -Wtype-limits \
#-DHAS_AUDIO -DHAS_AUDIO_CHOICE  -DTARGET_PHYS_ADDR_BITS=64 \

