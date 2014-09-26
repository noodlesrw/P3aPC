/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2009 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"

/* AND SDL video driver implementation; this is just enough to make an
 *  SDL-based application THINK it's got a working video driver, for
 *  applications that call SDL_Init(SDL_INIT_VIDEO) when they don't need it,
 *  and also for use as a collection of stubs when porting SDL to a new
 *  platform for which you haven't yet written a valid video driver.
 *
 * This is also a great way to determine bottlenecks: if you think that SDL
 *  is a performance problem for a given platform, enable this driver, and
 *  then see if your application runs faster without video overhead.
 *
 * Initial work by Ryan C. Gordon (icculus@icculus.org). A good portion
 *  of this was cut-and-pasted from Stephane Peter's work in the AAlib
 *  SDL video driver.  Renamed to "DUMMY" by Sam Lantinga.
 */

#include "SDL_video.h"
#include "SDL_mouse.h"
#include "../SDL_sysvideo.h"
#include "../SDL_pixels_c.h"
#include "../../events/SDL_events_c.h"

#include "SDL_andvideo.h"
#include "SDL_andevents_c.h"
#include "SDL_andmouse_c.h"
#include "libSDL_jni_libSDLproxy.h"
#include <pthread.h>
#include <android/log.h>
#include <signal.h>
#include <SDL.h>

#define ANDVID_DRIVER_NAME "android"

/* Initialization/Query functions */
static int AND_VideoInit(_THIS, SDL_PixelFormat *vformat);
static SDL_Rect **AND_ListModes(_THIS, SDL_PixelFormat *format, Uint32 flags);
static SDL_Surface *AND_SetVideoMode(_THIS, SDL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int AND_SetColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors);
static void AND_VideoQuit(_THIS);

/* Hardware surface functions */
static int AND_AllocHWSurface(_THIS, SDL_Surface *surface);
static int AND_LockHWSurface(_THIS, SDL_Surface *surface);
static void AND_UnlockHWSurface(_THIS, SDL_Surface *surface);
static void AND_FreeHWSurface(_THIS, SDL_Surface *surface);

/* etc. */
static void AND_UpdateRects(_THIS, int numrects, SDL_Rect *rects);

int AND_printf(const char *, ...);
int AND_puts(const char *);

static SDL_VideoDevice *AND_device=0;
static pthread_t __threadp=0;       // thread where main() is running
static int AND_IsRunning=1;         // False if app asks to terminate
JavaVM *gJavaVM=0;
jobject libSDLproxy=0;       // Java part inplementation ptr
static int volatile AND_Paused=0;   // 1 == pause screen updates (we've lost focus)
static jobject DirectBuffer=0;      // global pointer to ByteBuffer containing screen

/* AND driver bootstrap functions */

static int AND_Available(void)
{
	const char *envr = SDL_getenv("SDL_VIDEODRIVER");
	if ((envr) && (SDL_strcmp(envr, ANDVID_DRIVER_NAME) == 0)) {
		return(1);
	}

	return(1);  // we are always available if linked
}

static void AND_DeleteDevice(SDL_VideoDevice *device)
{
	SDL_free(device->hidden);
	SDL_free(device);
}

static SDL_VideoDevice *AND_CreateDevice(int devindex)
{
	SDL_VideoDevice *device;

	/* Initialize all variables that we clean on shutdown */
	device = (SDL_VideoDevice *)SDL_malloc(sizeof(SDL_VideoDevice));
	if ( device ) {
		SDL_memset(device, 0, (sizeof *device));
		device->hidden = (struct SDL_PrivateVideoData *)
				SDL_malloc((sizeof *device->hidden));
	}
	if ( (device == NULL) || (device->hidden == NULL) ) {
		SDL_OutOfMemory();
		if ( device ) {
			SDL_free(device);
		}
		return(0);
	}
	SDL_memset(device->hidden, 0, (sizeof *device->hidden));

	/* Set the function pointers */
	device->VideoInit = AND_VideoInit;
	device->ListModes = AND_ListModes;
	device->SetVideoMode = AND_SetVideoMode;
	device->CreateYUVOverlay = NULL;
	device->SetColors = AND_SetColors;
	device->UpdateRects = AND_UpdateRects;
	device->VideoQuit = AND_VideoQuit;
	device->AllocHWSurface = AND_AllocHWSurface;
	device->CheckHWBlit = NULL;
	device->FillHWRect = NULL;
	device->SetHWColorKey = NULL;
	device->SetHWAlpha = NULL;
	device->LockHWSurface = AND_LockHWSurface;
	device->UnlockHWSurface = AND_UnlockHWSurface;
	device->FlipHWSurface = NULL;
	device->FreeHWSurface = AND_FreeHWSurface;
	device->SetCaption = NULL;
	device->SetIcon = NULL;
	device->IconifyWindow = NULL;
	device->GrabInput = NULL;
	device->GetWMInfo = NULL;
    device->FreeWMCursor = AND_FreeWMCursor;
	device->CreateWMCursor = AND_CreateWMCursor;
	device->ShowWMCursor = AND_ShowWMCursor;
	device->MoveWMCursor = AND_MoveWMCursor;
	device->InitOSKeymap = AND_InitOSKeymap;
	device->PumpEvents = AND_PumpEvents;

	device->free = AND_DeleteDevice;

	return device;
}

VideoBootStrap AND_bootstrap = {
	ANDVID_DRIVER_NAME, 
	"SDL Android video driver",
	AND_Available, 
	AND_CreateDevice
};


int AND_VideoInit(_THIS, SDL_PixelFormat *vformat)
{
	/* we change this during the SDL_SetVideoMode implementation... */
	vformat->BitsPerPixel = 16;
	vformat->BytesPerPixel = 2;

	/* We're done! */
	return(0);
}

SDL_Rect **AND_ListModes(_THIS, SDL_PixelFormat *format, Uint32 flags)
{
    if(format->BitsPerPixel!=16)    // only 16 bpp supported
        return 0;
    return (SDL_Rect **) -1;
}

SDL_Surface *AND_SetVideoMode(_THIS, SDL_Surface *current,
				int width, int height, int bpp, Uint32 flags)
{
    jclass cls = 0;
    jmethodID midNotify = 0;
    int status;
    JNIEnv *env;
    int isAttached = 0;

    status = (*gJavaVM)->GetEnv(gJavaVM,(void **) &env, JNI_VERSION_1_4);
    if(status < 0) 
    {
//        AND_puts("callback_handler: failed to get JNI environment, assuming native thread");
        status = (*gJavaVM)->AttachCurrentThread(gJavaVM,&env, NULL);
        if(status < 0) 
        {
            AND_puts("callback_handler: failed to attach "
                 "current thread");
            return 0;
        }
        isAttached = 1;
    }

    if(cls==0)
    {
        cls=(*env)->GetObjectClass(env,libSDLproxy);    // local ref
        if(cls==0)
        {
            AND_puts("Can't find class libSDL.jni.libSDLproxy");
            if(isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
            return 0;
        }
    }

    if(midNotify==0)
    {
        midNotify=(*env)->GetStaticMethodID(env,cls,"javaSDLnotifymodechanged","(II)Ljava/nio/Buffer;");
        if(midNotify==0)
        {
            AND_puts("Can't find javaSDLnotifymodechanged in libSDL.jni.libSDLproxy");
            if(isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
            return 0;
        }
    }

    if(DirectBuffer)
    {
/*
        if(this->hidden->buffer)
        {
            SDL_memset(this->hidden->buffer, 0, this->hidden->w * this->hidden->pitch);
            AND_UpdateRects(this, 0, 0);   // clear the old screen before changing resolution
        }*/
        (*env)->DeleteGlobalRef(env, DirectBuffer);  // free old ref
    }

    DirectBuffer = (*env)->CallStaticObjectMethod(env,cls,midNotify,(jint)width,(jint)height,NULL);

    if(DirectBuffer==0)
    {
        AND_puts("Can't alloc memory for screen buffer");
        if(isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return 0;
    }

    DirectBuffer = (*env)->NewGlobalRef(env, DirectBuffer); // convert to global and keep it to avoid moving it with GC
    // I assume that this address never changes (it is true in current implementation)
    this->hidden->buffer = (*env)->GetDirectBufferAddress(env, DirectBuffer);  

    if(isAttached) 
        (*gJavaVM)->DetachCurrentThread(gJavaVM);

    if ( bpp!=16 )  // support 16 bit only, other modes would be emulated by SDL
    {
//    	SDL_SetError("only 16 bpp supported");
//        return 0;
        bpp=16;
    }

	if ( this==NULL || this->hidden==NULL || this->hidden->buffer==NULL ) {
		SDL_SetError("Couldn't allocate buffer for requested mode");
		return(NULL);
	}

 	/*printf("Setting mode %dx%d\n", width, height); */

	SDL_memset(this->hidden->buffer, 0, width * height * (bpp / 8));

	/* Allocate the new pixel format for the screen */
	if ( ! SDL_ReallocFormat(current, bpp, 0, 0, 0, 0) ) {
/*		SDL_free(this->hidden->buffer);   */
		this->hidden->buffer = NULL;
		SDL_SetError("Couldn't allocate new pixel format for requested mode");
		return(NULL);
	}

	/* Set up the new mode framebuffer */
	current->flags = flags & SDL_FULLSCREEN;
	this->hidden->w = current->w = width;
	this->hidden->h = current->h = height;
	current->pitch = current->w * (bpp / 8);
    this->hidden->pitch = current->pitch;
	current->pixels = this->hidden->buffer;

    AND_Paused = 0;

	/* We're done */
	return(current);
}

/* We don't actually allow hardware surfaces other than the main one */
static int AND_AllocHWSurface(_THIS, SDL_Surface *surface)
{
	return(-1);
}
static void AND_FreeHWSurface(_THIS, SDL_Surface *surface)
{
	return;
}

/* We need to wait for vertical retrace on page flipped displays */
static int AND_LockHWSurface(_THIS, SDL_Surface *surface)
{
	return(0);
}

static void AND_UnlockHWSurface(_THIS, SDL_Surface *surface)
{
	return;
}

void AND_SdlCleanup()
{
    AND_IsRunning=0;
    SDL_Quit();
    pthread_kill(__threadp, SIGKILL);
}

// Code is taken from win32 sdlmain

static void UnEscapeQuotes( char *arg )
{
	char *last = NULL;

	while( *arg ) {
		if( *arg == '"' && *last == '\\' ) {
			char *c_curr = arg;
			char *c_last = last;

			while( *c_curr ) {
				*c_last = *c_curr;
				c_last = c_curr;
				c_curr++;
			}
			*c_last = '\0';
		}
		last = arg;
		arg++;
	}
}

/* Parse a command line buffer into arguments */
static int ParseCommandLine(char *cmdline, char **argv)
{
	char *bufp;
	char *lastp = NULL;
	int argc, last_argc;

	argc = last_argc = 0;
	for ( bufp = cmdline; *bufp; ) {
		/* Skip leading whitespace */
		while ( isspace(*bufp) ) {
			++bufp;
		}
		/* Skip over argument */
		if ( *bufp == '"' ) {
			++bufp;
			if ( *bufp ) {
				if ( argv ) {
					argv[argc] = bufp;
				}
				++argc;
			}
			/* Skip over word */
			while ( *bufp && ( *bufp != '"' || *lastp == '\\' ) ) {
				lastp = bufp;
				++bufp;
			}
		} else {
			if ( *bufp ) {
				if ( argv ) {
					argv[argc] = bufp;
				}
				++argc;
			}
			/* Skip over word */
			while ( *bufp && ! isspace(*bufp) ) {
				++bufp;
			}
		}
		if ( *bufp ) {
			if ( argv ) {
				*bufp = '\0';
			}
			++bufp;
		}

		/* Strip out \ from \" sequences */
		if( argv && last_argc != argc ) {
			UnEscapeQuotes( argv[last_argc] );	
		}
		last_argc = argc;	
	}
	if ( argv ) {
		argv[argc] = NULL;
	}
	return(argc);
}

static void *ThreadProc(void* Param)
{
    int ret;
    // | SDL_INIT_TIMER | SDL_INIT_NOPARACHUTE
    /*if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 ) {
		SDL_SetError("SDL_Init() error");
        pthread_exit(0);
		return 0;
	}*/

    char **argv=0;
    int argc=0;
    char Buff[1024];
    char *cmdline=(char*)Param;
    if(cmdline==0 || cmdline[0]==0) cmdline="./sdl";

    //SDL_ShowCursor (SDL_FALSE);

    __android_log_print(ANDROID_LOG_DEBUG,"SDL","calling main(%s), curdir: %s\n",cmdline,getcwd(Buff,1024));

	argv = SDL_stack_alloc(char*, 256);    // max 256 params
	if ( argv == NULL ) {
        __android_log_print(ANDROID_LOG_DEBUG,"SDL","ParseCommandLine\n");
		return 0;
	}
    argc = ParseCommandLine(cmdline, argv);

    ret=main(argc,argv);

    free(Param);    // string is strdup-ed

    __android_log_print(ANDROID_LOG_DEBUG,"SDL","main() returned with %d\n",ret);

    AND_SdlCleanup();

    pthread_exit(0);
    return 0;
}

int AND_printf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap,fmt);
    return __android_log_vprint(ANDROID_LOG_WARN, "SDL", fmt, ap);
}

int AND_puts(const char *str)
{
    return AND_printf("%s\n",str);
}

int AND_CallMain(const char *cd, const char *arg)
{
    pthread_attr_t type;
    	
    AND_printf("JNI: passed data directory: '%s'\n",cd);

    chdir(cd);

    pthread_attr_init(&type);
    pthread_attr_setdetachstate(&type, PTHREAD_CREATE_JOINABLE);
    return pthread_create (&__threadp, &type, ThreadProc, (void*)strdup(arg));
}

/*
 * ScreenBuffer and buffer have the same orientation, depth and size
 * ScreenBuffer rotation/scaling is done on java-side, possibly using 
 * acceleration
 */
int AND_DoUpdates(unsigned char /*short int*/ *ScreenBuffer, int w, int h)
{
    if(AND_device==0)
        return AND_IsRunning;

    // we already paint to ScreenBuffer
    //memcpy( ScreenBuffer, AND_device->hidden->buffer, w*h*2);
    return AND_IsRunning;
}

void AND_PumpEvents(_THIS)
{
    while(AND_Paused)   // implement here pausing the program on lost focus
    {
        sched_yield();        
    }
}

// code is borrowed from http://android.wooyd.org/JNIExample/
static void AND_UpdateRects(_THIS, int numrects, SDL_Rect *rects)
{
    static jclass cls = 0;
    static jmethodID midNotify = 0;
    int status;
    JNIEnv *env;
    int isAttached = 0;
   
    AND_device=this;

    status = (*gJavaVM)->GetEnv(gJavaVM,(void **) &env, JNI_VERSION_1_4);
    if(status < 0) 
    {
//        AND_puts("callback_handler: failed to get JNI environment, assuming native thread");
        status = (*gJavaVM)->AttachCurrentThread(gJavaVM,&env, NULL);
        if(status < 0) 
        {
            AND_puts("callback_handler: failed to attach "
                 "current thread");
            return;
        }
        isAttached = 1;
    }

    if(cls==0)
    {
        cls=(*env)->GetObjectClass(env,libSDLproxy);    // local ref
        cls=(*env)->NewGlobalRef(env, cls);             // convert to global and keep it
        if(cls==0)
        {
            AND_puts("Can't find class libSDL.jni.libSDLproxy");
            if(isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
            return;
        }
    }

    if(midNotify==0)
    {
        midNotify=(*env)->GetStaticMethodID(env,cls,"javaSDLnotifyneedupdate","()V");
//        midNotify=(*env)->NewGlobalRef(env, midNotify);             // convert to global and keep it
        if(midNotify==0)
        {
            AND_puts("Can't find javaSDLnotifyneedupdate in libSDL.jni.libSDLproxy");
            if(isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
            return;
        }
    }

//    memcpy(this->hidden->buffer,this->hidden->buffer, this->hidden->h * this->hidden->pitch);
	//memset( this->hidden->buffer, 0x5555, this->hidden->w*this->hidden->h*2 );

    (*env)->CallStaticVoidMethod(env,cls,midNotify,NULL);

    if(isAttached) 
        (*gJavaVM)->DetachCurrentThread(gJavaVM);
}

int AND_SetColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors)
{
	/* do nothing of note. */
	return(1);
}

/* Note:  If we are terminated, this could be called in the middle of
   another SDL video routine -- notably UpdateRects.
*/
void AND_VideoQuit(_THIS)
{
    AND_IsRunning=0;
    AND_device=0;
	if (this->screen->pixels != NULL)
	{
/* FIXME: delete global ref		SDL_free(this->screen->pixels); */
		this->screen->pixels = NULL;
	}
}

JNIEXPORT jint JNICALL Java_libSDL_jni_libSDLproxy_libSDLmain
  (JNIEnv *env, jclass cls, jstring CurrDir, jstring Cmdline)
{
    jboolean iscopy;
    jint ret;
    const char *dir = (*env)->GetStringUTFChars(
                env, CurrDir, &iscopy);
    const char *arg = (*env)->GetStringUTFChars(
                env, Cmdline, &iscopy);
    ret = AND_CallMain(dir,arg);
    (*env)->ReleaseStringUTFChars(env, CurrDir, dir);
    return ret;
}

JNIEXPORT jint JNICALL Java_libSDL_jni_libSDLproxy_libSDLcheckloaded
  (JNIEnv *_env, jclass jc)
{
    return 2;
}

JNIEXPORT void JNICALL Java_libSDL_jni_libSDLproxy_libSDLcleanup
  (JNIEnv *env, jclass jc)
{
    AND_SdlCleanup();
    return;
}

JNIEXPORT void JNICALL Java_libSDL_jni_libSDLproxy_libSDLpause
  (JNIEnv *env, jclass jc)
{
    AND_Paused = 1;
    return;
}

JNIEXPORT void JNICALL Java_libSDL_jni_libSDLproxy_libSDLresume
  (JNIEnv *env, jclass jc)
{
    AND_Paused = 0;
    return;
}

/*JNIEXPORT jint JNICALL Java_libSDL_jni_libSDLproxy_libSDLupdatebuffer
  (JNIEnv *env, jclass jc, jintArray arr, jint w, jint h)*/
JNIEXPORT jint JNICALL Java_libSDL_jni_libSDLproxy_libSDLupdatebuffer
  (JNIEnv *env, jclass jc, jobject arr, jint w, jint h)
{
    jint ret=0;
    if(AND_device==0)
        return -1;

//    jint *Scr = (*env)->GetIntArrayElements(env, arr, 0);
    unsigned char /*short int*/ *Scr = (*env)->GetDirectBufferAddress(env, arr);
    ret=AND_DoUpdates(Scr, w, h);
//    (*env)->ReleaseIntArrayElements(env, arr, Scr, 0);
    return ret;
}

JNIEXPORT jint JNICALL Java_libSDL_jni_libSDLproxy_libSDLisportrait
  (JNIEnv *env, jclass jc)
{
    if( AND_device==0 )
        return -1;
    if ( AND_device->hidden==0 )
		return -1;
    return AND_device->hidden->w <= AND_device->hidden->h;
}


JNIEXPORT void JNICALL Java_libSDL_jni_libSDLproxy_libSDLnotifytouch
  (JNIEnv *env, jclass jc, jint flag, jint button, jint x, jint y)
{
    if(AND_device==0)
        return;

    switch (flag)
    {
    case 0:
        SDL_PrivateMouseMotion(0,0,x,y);    // ignore buttons, absolute coords
	    return;
	case 1:
	    SDL_PrivateMouseButton(SDL_PRESSED,button,0,0); // ignore coords
	    return;
	case 2:
	    SDL_PrivateMouseButton(SDL_RELEASED,button,0,0);
        return;
    };
}

JNIEXPORT void JNICALL Java_libSDL_jni_libSDLproxy_libSDLnotifytrackpad
  (JNIEnv *env, jclass jc, jint flag, jint button, jint x, jint y)
{
    switch (flag)
    {
    case 0:
        SDL_PrivateMouseMotion(0,1,x,y);    // ignore buttons, relative coords
	    return;
	case 1:
	    SDL_PrivateMouseButton(SDL_PRESSED,button,0,0); // ignore coords
	    return;
	case 2:
	    SDL_PrivateMouseButton(SDL_RELEASED,button,0,0);
        return;
    };
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv *env;
    gJavaVM = vm;
    jint retV=-1;
    if ((*vm)->GetEnv(vm,(void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        AND_puts("Failed to get the environment using GetEnv()");
        return -1;
    }

    jclass cls = (*env)->FindClass(env,"libSDL/jni/libSDLproxy");
    if(!cls) {
        AND_puts("Failed to get libSDLproxy class reference");
        return retV;
    }
    jmethodID constr = (*env)->GetMethodID(env,cls, "<init>", "()V");
    if(!constr) {
        AND_puts("Failed to get libSDLproxy constructor");
        return retV;
    }
    jobject obj = (*env)->NewObject(env,cls, constr);
    if(!obj) {
        AND_puts("Failed to create a libSDLproxy object");
        return retV;
    }
    libSDLproxy = (*env)->NewGlobalRef(env,obj);

    AND_puts("JNI_OnLoad: requesting JNI_VERSION_1_4");

    return JNI_VERSION_1_4;
}

void JNI_OnUnload(JavaVM* vm, void* reserved)
{
}
