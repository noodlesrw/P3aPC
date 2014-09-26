#!/bin/bash

PATH=/home/nathan/android-ndk-r7/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin:$PATH \
arm-linux-androideabi-gcc -march=armv6j \
-D_FORTIFY_SOURCE=2 -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64 \
-D_LARGEFILE_SOURCE -Wstrict-prototypes -Wredundant-decls -Wall -Wundef \
-Wwrite-strings -Wmissing-prototypes -fno-strict-aliasing \
-mandroid -nostdlib \
-I/home/nathan/android/gcc4x/arm-linux-elf/include/SDL \
-I/home/nathan/android/gcc4x/arm-linux-elf/include \
--no-exceptions \
-g -nostdlib /home/nathan/android/gcc4x/arm-linux-elf/lib/crt0.o \
-L/home/nathan/android/gcc4x/lib \
-L/home/nathan/android/gcc4x/arm-linux-elf/lib \
-L/usr/arm-unknown-linux-gnueabi/usr/lib \
-Bstatic -l:libglib-2.0.a -l:libSDL.a -l:libstdc++.a -l:libz.a \
-Bdynamic -ldl -lc -lm \
zc.c -o zc
