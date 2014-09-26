#!/bin/bash

#toolpath=/home/nathan/android-ndk-r7/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin
#toolpath+=:/home/nathan/android-ndk-r7/platforms/android-5/arch-arm/usr/lib

#xprefix=arm-linux-androideabi-

#usrpath=/home/nathan/android-ndk-r7/platforms/android-5/arch-arm/usr
#crtpath=/home/nathan/android-ndk-r7/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/lib/gcc/arm-linux-androideabi/4.4.3
#usrpath=/home/nathan/android/gcc4x/arm-linux-elf
#crtpath=/home/nathan/android/gcc4x/lib/gcc/arm-linux-elf/4.5.0
toolpath=.
xprefix=arm-unknown-linux-gnueabi-


PATH=$toolpath:$PATH ./configure --target-list="i386-softmmu" \
--static --enable-sdl --disable-kvm --enable-io-thread \
--disable-slirp --disable-vde --disable-guest-agent \
--disable-vnc --audio-drv-list=sdl --disable-xen \
--disable-brlapi --disable-curses --disable-curl \
--disable-fdt --disable-bluez --disable-user \
--disable-uuid --disable-attr --disable-blobs \
--disable-docs --disable-spice --disable-rbd \
--disable-smartcard --disable-usb-redir --disable-nptl \
--disable-vhost-net \
--cross-prefix=$xprefix
