#!/bin/bash

NDKROOT=/home/nathan/android-ndk-r7

NDKTOOLPATH=toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin
#/home/nathan/android-ndk-r7/platforms/android-5/arch-arm/usr/include

SDLPATH="/home/nathan/android/SDL-1.2.14"

CFLAGS="-O2 -Wl,--export-dynamic,--discard-none -I$SDLPATH/Inst/include \
   -I$NDKROOT/platforms/android-5/arch-arm/usr/include \
   -L$NDKROOT/platforms/android-5/arch-arm/usr/lib \
   --sysroot=$NDKROOT/platforms/android-5/arch-arm "
CPPFLAGS="$CFLAGS"
CXXFLAGS="$CFLAGS"
LDFLAGS="-I$SDLPATH/Inst/lib \
   -L$NDKROOT/platforms/android-5/arch-arm/usr/lib \
   --sysroot=$NDKROOT/platforms/android-5/arch-arm -lz -lc -lm "
CFLAGS="-lz"
LDFLAGS="-lz"
#PATH="$SDLPATH:$SDLPATH/Inst/bin:$NDKROOT/$NDKTOOLPATH:$PATH"
CFLAGS="$CFLAGS" \
LDFLAGS="$LDFLAGS" \
./configure --target-list="i386-softmmu"
#--enable-sdl --disable-vnc --enable-io-thread \
#--disable-slirp --disable-vde --disable-guest-agent \
#--disable-kvm --audio-drv-list=sdl --disable-xen --disable-brlapi \
#--disable-curses --disable-curl --disable-fdt --disable-bluez --disable-user \
#--disable-uuid --disable-attr --disable-blobs --disable-docs --disable-spice \
#--disable-rbd --disable-smartcard --disable-usb-redir --disable-nptl \
#--disable-vhost-net

#--cross-prefix=arm-linux-androideabi- \
