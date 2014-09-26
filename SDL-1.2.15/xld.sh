PATH="/home/nathan/android-ndk-r8e/toolchains/arm-linux-androideabi-4.7/prebuilt/linux-x86/bin:$PATH" \
arm-linux-androideabi-ld --sysroot=/tmp/ndk-andrewhsieh/build/toolchain/prefix/sysroot \
--eh-frame-hdr -shared -dynamic-linker /system/bin/linker -X -m armelf_linux_eabi \
-Bsymbolic -z noexecstack -z relro -z now -o build/.libs/libSDL-1.2.so.0.11.4 \
/home/nathan/android-ndk-r8e/platforms/android-5/arch-arm/usr/lib/crtbegin_so.o \
-L/home/nathan/android-ndk-r8e/toolchains/arm-linux-androideabi-4.7/prebuilt/linux-x86/bin/../lib/gcc/arm-linux-androideabi/4.7 \
-L/home/nathan/android-ndk-r8e/toolchains/arm-linux-androideabi-4.7/prebuilt/linux-x86/bin/../lib/gcc \
-L/home/nathan/android-ndk-r8e/toolchains/arm-linux-androideabi-4.7/prebuilt/linux-x86/bin/../lib/gcc/arm-linux-androideabi/4.7/../../../../arm-linux-androideabi/lib \
build/.libs/SDL.o build/.libs/SDL_error.o build/.libs/SDL_fatal.o build/.libs/SDL_audio.o \
build/.libs/SDL_audiocvt.o build/.libs/SDL_audiodev.o build/.libs/SDL_mixer.o \
build/.libs/SDL_mixer_MMX.o build/.libs/SDL_mixer_MMX_VC.o build/.libs/SDL_mixer_m68k.o \
build/.libs/SDL_wave.o build/.libs/SDL_cdrom.o build/.libs/SDL_cpuinfo.o \
build/.libs/SDL_active.o build/.libs/SDL_events.o build/.libs/SDL_expose.o \
build/.libs/SDL_keyboard.o build/.libs/SDL_mouse.o build/.libs/SDL_quit.o \
build/.libs/SDL_resize.o build/.libs/SDL_rwops.o build/.libs/SDL_getenv.o \
build/.libs/SDL_iconv.o build/.libs/SDL_malloc.o build/.libs/SDL_qsort.o \
build/.libs/SDL_stdlib.o build/.libs/SDL_string.o build/.libs/SDL_thread.o \
build/.libs/SDL_timer.o build/.libs/SDL_RLEaccel.o build/.libs/SDL_blit.o \
build/.libs/SDL_blit_0.o build/.libs/SDL_blit_1.o build/.libs/SDL_blit_A.o \
build/.libs/SDL_blit_N.o build/.libs/SDL_bmp.o build/.libs/SDL_cursor.o \
build/.libs/SDL_gamma.o build/.libs/SDL_pixels.o build/.libs/SDL_stretch.o \
build/.libs/SDL_surface.o build/.libs/SDL_video.o build/.libs/SDL_yuv.o \
build/.libs/SDL_yuv_mmx.o build/.libs/SDL_yuv_sw.o build/.libs/SDL_joystick.o \
build/.libs/SDL_nullevents.o build/.libs/SDL_nullmouse.o build/.libs/SDL_nullvideo.o \
build/.libs/SDL_diskaudio.o build/.libs/SDL_dummyaudio.o build/.libs/SDL_sysloadso.o \
build/.libs/SDL_fb3dfx.o build/.libs/SDL_fbelo.o build/.libs/SDL_fbevents.o \
build/.libs/SDL_fbmatrox.o build/.libs/SDL_fbmouse.o build/.libs/SDL_fbriva.o \
build/.libs/SDL_fbvideo.o build/.libs/SDL_sysjoystick.o build/.libs/SDL_syscdrom.o \
build/.libs/SDL_systimer.o build/.libs/SDL_andevents.o build/.libs/SDL_andmouse.o \
build/.libs/SDL_andvideo.o build/.libs/SDL_andaudio.o build/.libs/SDL_syscond.o \
build/.libs/SDL_sysmutex.o build/.libs/SDL_syssem.o build/.libs/SDL_systhread.o \
-lm -ldl --sysroot=/home/nathan/android-ndk-r8e/platforms/android-5/arch-arm \
-L/home/nathan/android-ndk-r8e/platforms/android-5/arch-arm/usr/lib/ \
-v -soname libSDL-1.2.so.0 -lgcc -lc -ldl -lgcc \
/home/nathan/android-ndk-r8e/platforms/android-5/arch-arm/usr/lib/crtend_so.o
