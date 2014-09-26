#/bin/bash

#BFLAGS="-mbionic -mandroid -static \
#--sysroot=/home/nathan/android-ndk-r7/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86 \
#-I/home/nathan/android-ndk-r7/platforms/android-5/arch-arm/usr/include " \
#/home/nathan/android-ndk-r7/toolchains/arm-linux-androideabi-4.4.3/\
#prebuilt/linux-x86/bin/arm-linux-androideabi-gcc $BFLAGS -v tt.c -o tt
#-isystem /home/nathan/android-ndk-r7/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/ \


BFLAGS="-D_REENTRANT -static -I/home/nathan/android-ndk-r7/platforms/android-5/arch-arm/usr/include \
--sysroot=/home/nathan/android-ndk-r7/platforms/android-5/arch-arm/usr"
/home/nathan/android-ndk-r7/toolchains/arm-linux-androideabi-4.4.3/\
prebuilt/linux-x86/bin/arm-linux-androideabi-gcc $BFLAGS -v tt.c -o tt


#arm-unknown-linux-gnueabi-gcc $BFLAGS -v tt.c -o tt
