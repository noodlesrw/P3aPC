#!/bin/bash

XPATH=""

CFLAGS="-g "

CPPFLAGS="$CFLAGS"
CXXFLAGS="$CFLAGS"

LDFLAGS="-g -lz -lc -lm"

PATH="$XPATH:$PATH" \
CFLAGS="$CFLAGS" \
LDFLAGS="$LDFLAGS" \
./configure --target-list="i386-softmmu" \
--enable-sdl --enable-io-thread \
--disable-vnc --disable-vnc-thread --disable-linux-aio \
--disable-slirp --disable-vde --disable-guest-agent \
--disable-kvm --audio-drv-list=sdl --disable-xen --disable-brlapi \
--disable-curses --disable-curl --disable-fdt --disable-bluez --disable-user \
--disable-uuid --disable-attr --disable-blobs --disable-docs --disable-spice \
--disable-rbd --disable-smartcard --disable-usb-redir --disable-nptl \
--enable-vhost-net --disable-system

#--enable-io-thread 

#NDKROOT=/home/nathan/android-ndk-r7

#NDKTOOLPATH=toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin
#/home/nathan/android-ndk-r7/platforms/android-5/arch-arm/usr/include

#SDLPATH="/home/nathan/android/SDL-1.2.15"

#CFLAGS="-g -mandroid -Wl,--export-dynamic,--discard-none -I$SDLPATH/Inst/include \
#   --sysroot=$NDKROOT/platforms/android-5/arch-arm \
#   -I$NDKROOT/platforms/android-5/arch-arm/usr/include \
#   -L$NDKROOT/platforms/android-5/arch-arm/usr/lib "
#CPPFLAGS="$CFLAGS"
#CXXFLAGS="$CFLAGS"
#LDFLAGS="-g -mandroid -I$SDLPATH/Inst/lib \
#   --sysroot=$NDKROOT/platforms/android-5/arch-arm \
#   -L$NDKROOT/platforms/android-5/arch-arm/usr/lib \
#   -lz -lc -lm -llog "

