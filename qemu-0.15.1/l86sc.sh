#!/bin/bash

#NDKROOT="/home/nathan/android/adt/ndk"

#NDKTOOLPATH="toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86"

#NDKSYSROOT="$NDKROOT/platforms/android-8/arch-arm"

#SDLPATH="/home/nathan/SDL-1.2.15"

#CFLAGS="--sysroot $NDKSYSROOT \
#    -I$SDLPATH/Inst/include \
#    -mandroid -march=armv7-a -mfloat-abi=softfp -mfpu=vfp \
#    -isysroot $NDKSYSROOT \
#    -isystem $NDKSYSROOT \
#    -I$NDKSYSROOT/usr/include \
#    -L$NDKSYSROOT/usr/lib \
#	-lz -llog -lc -lm \
#	-L$SDLPATH/Inst/lib -l:libSDL.a "

#LDFLAGS="--sysroot=$NDKSYSROOT \
#    -L$NDKSYSROOT/usr/lib \
#	-lz -llog -lc -lm -L$SDLPATH/Inst/lib -l:libSDL.a "

#CFLAGS="$CFLAGS" \
#LDFLAGS="$LDFLAGS" \
#PATH="$SDLPATH:$SDLPATH/Inst/bin:$NDKROOT:$NDKROOT/$NDKTOOLPATH/bin:$NDKROOT/$NDKTOOLPATH:$PATH" \

CFLAGS="-lz -lpthread -lutil -lrt"

CFLAGS="$CFLAGS" \
./configure --target-list="i386-softmmu" \
--enable-system --enable-vhost-net \
--enable-sdl --audio-drv-list=sdl --enable-io-thread \
--enable-vnc --enable-vnc-thread \
--disable-guest-agent --disable-kvm \
--disable-docs --disable-spice --disable-smartcard \
--disable-bluez --disable-nptl \
--disable-curl --disable-fdt --disable-check-utests \
--disable-xen --disable-brlapi --disable-curses \
--disable-uuid --disable-vde --disable-attr --disable-blobs

#--cross-prefix=arm-linux-androideabi- \-mfloat-abi=softfp -mfpu=vfp 
#--disable-vhost-net --audio-drv-list=sdl 
#--disable-linux-aio  --enable-io-thread
#--enable-io-thread \
#--disable-xen --disable-brlapi --disable-vnc --disable-curses \
#--disable-curl --disable-fdt --disable-check-utests \
#--disable-bluez --disable-slirp --disable-nptl \
#--disable-user --disable-linux-user --disable-guest-base --disable-user-pie \
#--disable-uuid --disable-vde --disable-linux-aio --disable-attr --disable-blobs \
#--disable-docs --disable-spice --disable-smartcard --disable-usb-redir \
#

#make              make
#install           install
#python            python
#smbd              /usr/sbin/smbd
#host CPU          i386
#host big endian   no
#target list       i386-softmmu
#tcg debug enabled no
#Mon debug enabled no
#gprof enabled     no
#sparse enabled    no
#strip binaries    yes
#profiler          no
#static build      no
#-Werror enabled   no
#SDL support       yes
#curses support    no
#curl support      no
#check support     no
#mingw32 support   no
#Audio drivers     sdl
#Extra audio cards ac97 es1370 sb16 hda
#Block whitelist   
#Mixer emulation   no
#VNC support       yes
#VNC TLS support   yes
#VNC SASL support  no
#VNC JPEG support  yes
#VNC PNG support   yes
#VNC thread        yes
#xen support       no
#brlapi support    no
#bluez  support    no
#Documentation     no
#NPTL support      no
#GUEST_BASE        yes
#PIE user targets  no
#vde support       no
#IO thread         yes
#Linux AIO support no
#ATTR/XATTR support no
#Install blobs     no
#KVM support       no
#fdt support       no
#preadv support    yes
#fdatasync         yes
#madvise           yes
#posix_madvise     yes
#uuid support      no
#vhost-net support yes
#Trace backend     nop
#Trace output file trace-<pid>
#spice support     no
#rbd support       no
#xfsctl support    no
#nss used          no
#usb net redir     no
#OpenGL support    yes
#build guest agent no
