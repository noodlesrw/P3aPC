#!/bin/bash

#NDKROOT="/home/nathan/android/adt/ndk"

#NDKTOOLPATH="toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86"

#NDKSYSROOT="$NDKROOT/platforms/android-8/arch-arm"

#SDLPATH="/home/nathan/SDL-1.2.15"

#CFLAGS="--sysroot $NDKSYSROOT \
#    -I$SDLPATH/Inst/include \
#    -mandroid \
#    -isysroot $NDKSYSROOT \
#    -isystem $NDKSYSROOT \
#    -I$NDKSYSROOT/usr/include \
#    -L$NDKSYSROOT/usr/lib \
#	-lz -llog -lc -lm \
#	-L$SDLPATH/Inst/lib -l:libSDL.a -shared "

#LDFLAGS="--sysroot=$NDKSYSROOT \
#    -L$NDKSYSROOT/usr/lib \
#	-lz -llog -lc -lm -L$SDLPATH/Inst/lib -l:libSDL.a "

#PATH="$SDLPATH:$SDLPATH/Inst/bin:$NDKROOT:$NDKROOT/$NDKTOOLPATH/bin:$NDKROOT/$NDKTOOLPATH:$PATH" \

make

#if [ -e i386-softmmu/qemu ]; then
#PATH="$SDLPATH:$SDLPATH/Inst/bin:$NDKROOT:$NDKROOT/$NDKTOOLPATH/bin:$NDKROOT/$NDKTOOLPATH:$PATH" \
#arm-linux-androideabi-strip i386-softmmu/qemu
#cp i386-softmmu/qemu /home/nathan/SDLapp/libs/armeabi/libnativeapp.so
#cp i386-softmmu/qemu i386-softmmu/libnativeapp.so
#fi
