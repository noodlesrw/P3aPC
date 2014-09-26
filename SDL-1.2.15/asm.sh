#!/bin/bash

if [ -e build/.libs/libSDL.a ]; then
	rm build/.libs/libSDL.a
fi

NDKROOT="/home/nathan/android-ndk-r8e"

NDKTOOLPATH="toolchains/arm-linux-androideabi-4.7/prebuilt/linux-x86"

NDKSYSROOT="$NDKROOT/platforms/android-5/arch-arm"

SDLPATH="/home/nathan/android/SDL-1.2.15"

CFLAGS="--sysroot=$NDKSYSROOT \
	-DNO_MNTENT -UHAVE_ICONV -DNO_MALLINFO=1 \
    -mandroid -fomit-frame-pointer -march=armv7-a \
    -I$NDKSYSROOT/usr/include \
    -L$NDKSYSROOT/usr/lib "

LDFLAGS="--sysroot=$NDKSYSROOT -L$NDKSYSROOT/usr/lib -Wl,-v -Wl,-E "

PATH="$NDKROOT:$NDKROOT/$NDKTOOLPATH/bin:$NDKROOT/$NDKTOOLPATH:$SDLPATH:$PATH"

PATH="$PATH" \
CC=arm-linux-androideabi-gcc \
make

if [ -e build/.libs/libSDL.a ]; then

	chmod +x ./Inst/bin/sdl-config
	make install DESTDIR=`pwd`/Inst
	cp -R Inst/home/nathan/android/SDL-1.2.15/Inst/* Inst/
	
fi



#CFLAGS=" -DNO_MNTENT -UHAVE_ICONV -DNO_MALLINFO=1 -Wl,-v \
#    -mandroid -fomit-frame-pointer -march=armv7-a \
#	--sysroot=$NDKSYSROOT \
#	-nostdlib \
#	-Wl,--sysroot=$NDKSYSROOT -Wl,-L$NDKSYSROOT/usr/lib "

#CPPFLAGS="$CFLAGS"
#CXXFLAGS="$CFLAGS"
#LDFLAGS="--sysroot=$NDKROOT/platforms/android-5/arch-arm -rpath-link /usr/lib "

#chmod +x ./Inst/bin/sdl-config
#CXXFLAGS="$CXFLAGS" \LDFLAGS="$LDFLAGS" 
#CFLAGS="$CFLAGS" 
#CFLAGS="$CFLAGS" 
#PATH="$NDKROOT:$NDKROOT/$NDKTOOLPATH/bin:$NDKROOT/$NDKTOOLPATH:$SDLPATH:$NDKSYSROOT/usr/lib:$PATH"

