#!/bin/bash

if [ -e build/.libs/libSDL.a ]; then
	rm build/.libs/libSDL.a
fi

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


#chmod +x ./Inst/bin/sdl-config

PATH="$PATH" \
CFLAGS="$CFLAGS" \
CC=arm-linux-androideabi-gcc \
make

if [ -e build/.libs/libSDL.a ]; then

	chmod +x ./Inst/bin/sdl-config
	make install DESTDIR=`pwd`/Inst
	cp -R Inst/home/nathan/SDL-1.2.15/Inst/* Inst/
	
fi
