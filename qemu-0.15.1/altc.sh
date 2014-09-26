#!/bin/bash

#make clean

SDLPATH="/home/nathan/android/SDL-1.2.15"

CFLAGS=" -I/usr/arm-unknown-linux-gnueabi/usr/lib -l:libc.a -l:libz.a -l:libSDL.a"

PATH="$SDLPATH:$SDLPATH/Inst/bin:$PATH" \
./configure --target-list="i386-softmmu" \
--cross-prefix=arm-unknown-linux-gnueabi- --static \
--enable-sdl --enable-system --enable-io-thread \
--audio-drv-list=sdl --enable-guest-base \
--enable-vnc --enable-vnc-thread --disable-linux-aio \
--disable-slirp --disable-vde --disable-guest-agent \
--disable-kvm --disable-xen --disable-brlapi \
--disable-curses --disable-curl --disable-fdt --disable-bluez --disable-user \
--disable-uuid --disable-attr --disable-blobs --disable-docs --disable-spice \
--disable-rbd --disable-smartcard --disable-usb-redir --disable-nptl \
--disable-vhost-net

#--enable-io-thread 
#--disable-system 
