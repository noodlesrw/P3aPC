#!/bin/bash

if [ -e build/.libs/libSDL.a ]; then
	rm build/.libs/libSDL.a
fi

#CPPFLAGS="--sysroot=$NDKROOT/platforms/android-5/arch-arm " \
#   CFLAGS="--sysroot=$NDKROOT/platforms/android-5/arch-arm \
#   -mandroid -DNO_MNTENT " \
#   CXXFLAGS="--sysroot=$NDKROOT/platforms/android-5/arch-arm " \
#   LDFLAGS="--sysroot=$NDKROOT/platforms/android-5/arch-arm \
#   -mandroid \
#   -L/home/nathan/android-ndk-r7/platforms/android-5/arch-arm/usr/lib " \

CC=arm-unknown-linux-gnueabi-gcc \
make

if [ -e build/.libs/libSDL.a ]; then

	chmod +x ./Inst/bin/sdl-config
	make install DESTDIR=`pwd`/Inst
	cp -R Inst/home/nathan/android/SDL-1.2.15/Inst/* Inst/
	
fi
