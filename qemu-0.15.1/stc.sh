#!/bin/bash
#/usr/arm-unknown-linux-gnueabi/usr
NDKROOT=/usr/arm-unknown-linux-gnueabi/
#/home/nathan/android-ndk-r7

#NDKTOOLPATH=toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin
#/home/nathan/android-ndk-r7/platforms/android-5/arch-arm/usr/include

SDLPATH="/home/nathan/android/SDL-1.2.14"

CFLAGS="-O2 -Wl,--export-dynamic,--discard-none -I$SDLPATH/Inst/include \
   -I$NDKROOT/usr/include \
   -L$NDKROOT/usr/lib \
   --sysroot=$NDKROOT "
CPPFLAGS="$CFLAGS"
CXXFLAGS="$CFLAGS"
LDFLAGS="-I$SDLPATH/Inst/lib \
   -L$NDKROOT/usr/lib \
   --sysroot=$NDKROOT -lz -lc -lm "

PATH="$SDLPATH:$SDLPATH/Inst/bin:$PATH" \
CFLAGS="$CFLAGS" \
LDFLAGS="$LDFLAGS" \
./configure --target-list="i386-softmmu" \
--cross-prefix=arm-unknown-linux-gnueabi- \
--enable-sdl --enable-vnc --enable-io-thread --enable-system \
--disable-slirp --disable-vde --disable-guest-agent \
--disable-kvm --audio-drv-list=sdl --disable-xen --disable-brlapi \
--disable-curses --disable-curl --disable-fdt --disable-bluez --disable-user \
--disable-uuid --disable-attr --disable-blobs --disable-docs --disable-spice \
--disable-rbd --disable-smartcard --disable-usb-redir --disable-nptl \
--disable-vhost-net

#--enable-io-thread 
