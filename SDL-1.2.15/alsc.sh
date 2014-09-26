#!/bin/bash

#clean the slate ready
make clean

chmod +x ./Inst/bin/sdl-config


CFLAGS="$CFLAGS " \
./configure \
--host=arm-unknown-linux-gnueabi \
--prefix=/home/nathan/android/SDL-1.2.15/Inst \
--enable-oss=no --enable-video-x11=no --enable-pulseaudio=no 
#\
#--enable-libc 
#\
#--enable-nasm=no --enable-oss=no --enable-video-x11=yes --enable-video-opengl=yes \
#--enable-video-x11-vm=yes --enable-video-x11-xv=yes --enable-video-x11-xrandr=yes \
#--enable-video-directfb=yes --enable-video-fbcon=yes --enable-video-dummy=no \
#--enable-pth=no --enable-pthreads=no --enable-pthread-sem=no \
#--enable-sdl-dlopen=no --enable-alsa=yes --enable-static=yes \
#--enable-pulseaudio=no --enable-pulseaudio-shared=no \
#--enable-audio=yes --enable-video=yes --enable-events=no --enable-joystick=yes \
#--enable-cdrom=yes  --enable-threads=yes --enable-timers=yes \
#--enable-video-dummy=yes --enable-directx=no
