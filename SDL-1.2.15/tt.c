
/*
CFLAGS="-mbionic -mandroid -static \
--sysroot=/home/nathan/android-ndk-r7/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86 \
-I/home/nathan/android-ndk-r7/platforms/android-5/arch-arm/usr/include " \
/home/nathan/android-ndk-r7/toolchains/arm-linux-androideabi-4.4.3/\
prebuilt/linux-x86/bin/arm-linux-androideabi-gcc $CFLAGS -v tt.c -o tt
   
*/   

/* end confdefs.h*/

#include <pthread.h>

int
main ()
{

                  pthread_mutexattr_t attr;
                  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

  ;
  return 0;
}

/* end confdefs.h  */
/*
                  #include <pthread.h>

int
main ()
{

                  pthread_mutexattr_t attr;
                  pthread_mutexattr_setkind_np(&attr, PTHREAD_MUTEX_RECURSIVE_NP);

  ;
  return 0;
}
*/
