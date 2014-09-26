#!/bin/bash

NDKROOT=/home/nathan/android-ndk-r7

NDKTOOLPATH=toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin

# And then (unmodified)
#
#export AR=$NDKROOT/$NDKTOOLPATH/arm-linux-androideabi-ar
#export CPP=$NDKROOT/$NDKTOOLPATH/arm-linux-androideabi-cpp
#export LD=$NDKROOT/$NDKTOOLPATH/arm-linux-androideabi-ld
#export CXX=$NDKROOT/$NDKTOOLPATH/arm-linux-androideabi-g++
#export CC=$NDKROOT/$NDKTOOLPATH/arm-linux-androideabi-gcc

PATH=/home/nathan/android/SDL-1.2.14/Inst/bin:$NDKROOT/$NDKTOOLPATH:$PATH \
CPPFLAGS="--sysroot=$NDKROOT/platforms/android-5/arch-arm \
          -I$NDKROOT/sources/cxx-stl/gnu-libstdc++/include/ \
          -I$NDKROOT/sources/cxx-stl/gnu-libstdc++/libs/armeabi/include \
          -I$NDKROOT/sources/cxx-stl/gnu-libstdc++/include/backward" \
   CFLAGS="-mandroid --sysroot=$NDKROOT/platforms/android-5/arch-arm \
          -I/usr/arm-unknown-linux-gnueabi/usr/include" \
   CXXFLAGS="--sysroot=$NDKROOT/platforms/android-5/arch-arm \
             -I$NDKROOT/sources/cxx-stl/gnu-libstdc++/include/ \
             -I$NDKROOT/sources/cxx-stl/gnu-libstdc++/libs/armeabi/include \
              -I$NDKROOT/sources/cxx-stl/gnu-libstdc++/include/backward" \
   LDFLAGS="--sysroot=$NDKROOT/platforms/android-5/arch-arm \
            -L$NDKROOT/sources/cxx-stl/gnu-libstdc++/libs/armeabi/" \
./configure --target-list="i386-softmmu" \
--cross-prefix=arm-linux-androideabi- \
--enable-sdl --cpu=armv6j --disable-kvm --enable-io-thread \
--disable-slirp --disable-vde --disable-guest-agent \
--disable-vnc --audio-drv-list=sdl --disable-xen --disable-brlapi \
--disable-curses --disable-curl --disable-fdt --disable-bluez --disable-user \
--disable-uuid --disable-attr --disable-blobs --disable-docs --disable-spice \
--disable-rbd --disable-smartcard --disable-usb-redir --disable-nptl \
--disable-vhost-net \
--extra-ldflags="-Bdynamic -L/home/nathan/android-ndk-r7/platforms/android-5/arch-arm/usr/lib -lm -ldl -lc -lz"
