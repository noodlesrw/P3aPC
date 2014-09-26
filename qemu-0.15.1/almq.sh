#!/bin/bash

SDLPATH="/home/nathan/android/SDL-1.2.15"

PATH=$SDLPATH:$SDLPATH/Inst/bin:$PATH \
LDFLAGS="$LDFLAGS -l:libc.a -l:libz.a -l:libSDL.a" CC=arm-unknown-linux-gnueabi-gcc \
make $1

if [ -e i386-softmmu/qemu ]; then
arm-unknown-linux-gnueabi-strip i386-softmmu/qemu
cp qemu /home/nathan/android/SDLapp/libs/armeabi/libnativeapp.so
fi
