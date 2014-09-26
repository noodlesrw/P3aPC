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

static SDL_VideoDevice *AND_device=0;
static pthread_t __threadp=0;       // thread where main() is running
static int AND_IsRunning=1;         // False if app asks to terminate
static JavaVM *gJavaVM=0;
static jobject libSDLproxy=0;       // Java part inplementation ptr
static int volatile AND_Paused=0;   // 1 == pause screen updates (we've lost focus)

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
	ANDVID_DRIVER_NAME, "SDL Android video driver",
	AND_Available, AND_CreateDevice
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
    if ( bpp!=16 )  // support 16 bit only, other modes would be emulated by SDL
    {
//    	SDL_SetError("only 16 bpp supported");
//        return 0;
        bpp=16;
    }

	if ( this->hidden->buffer ) {
    	SDL_free( this->hidden->buffer );
	}

    this->hidden->buffer = SDL_malloc(width * height * (bpp / 8));
	if ( ! this->hidden->buffer ) {
		SDL_SetError("Couldn't allocate buffer for requested mode");
		return(NULL);
	}

/* 	printf("Setting mode %dx%d\n", width, height); */

	SDL_memset(this->hidden->buffer, 0, width * height * (bpp / 8));

	/* Allocate the new pixel format for the screen */
	if ( ! SDL_ReallocFormat(current, bpp, 0, 0, 0, 0) ) {
		SDL_free(this->hidden->buffer);
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

static void *ThreadProc(void* Param)
{
    int ret;
    if ( SDL_Init(0) < 0 ) {
        __android_log_print(ANDROID_LOG_DEBUG,"SDL","SDL_Init() error\n");
		SDL_SetError("SDL_Init() error");
        pthread_exit(0);
		return 0;
	}
    char *argv[]={"./sdlapp",0,0};
    char Buff[1024];

    SDL_ShowCursor (SDL_FALSE);

    __android_log_print(ANDROID_LOG_DEBUG,"SDL","calling main(), curdir: %s\n",getcwd(Buff,1024));

    ret=main(1,argv);

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

int AND_CallMain(const char *cd)
{
    pthread_attr_t type;

    AND_printf("JNI: passed data directory: '%s'\n",cd);

    chdir(cd);

    pthread_attr_init(&type);
    pthread_attr_setdetachstate(&type, PTHREAD_CREATE_JOINABLE);    
    pthread_create (&__threadp, &type, ThreadProc, (void*)cd);
}

// downscale image to ScreenBuffer
#pragma pack(1)
typedef struct tColorRGBA
{
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
} tColorRGBA;
#pragma pack()

int AND_DrawDownscale(short int *ScreenBuffer, int w, int h)
{

    return 0;
}

static void memcpyDbl(short int *dst,short int *src,int count)
{
    count/=4;   // 4 == 2 bytes in pixel and size is doubled
    while(count-->0)
    {
        *(dst++)=*src;
        *(dst++)=*(src++);
    }
}

// double pixels, useful for 320x240 modes on 640x480+ devices
int AND_DrawDoublescale(short int *ScreenBuffer, int w, int h)
{
    int buffW=AND_device->hidden->w*2, buffH=AND_device->hidden->h*2;
    short int *buffSrc=(short int*)AND_device->hidden->buffer;
    short int *buffDst=ScreenBuffer+(w-buffW)/2+((h-buffH)/2)*w;
//    memset(ScreenBuffer,0,w*h*4);   // not optimal for speed
    int i;
    for(i=0; i<buffH; i++)
    {
        memcpyDbl(buffDst,buffSrc,2*buffW);
        buffDst+=w; i++;
        memcpyDbl(buffDst,buffSrc,2*buffW);
        buffDst+=w; 
        buffSrc+=buffW/2;
    }
}

// do not scale, add blank lines top/left/rigth/bottom
int AND_DrawNoscale(short int *ScreenBuffer, int w, int h)
{
    int buffW=AND_device->hidden->w, buffH=AND_device->hidden->h;
    short int *buffSrc=(short int*)AND_device->hidden->buffer;
    short int *buffDst=ScreenBuffer+(w-buffW)/2+((h-buffH)/2)*w;
//    memset(ScreenBuffer,0,w*h*4);   // not optimal for speed
    int i;
    for(i=0; i<buffH; i++)
    {
        memcpy(buffDst,buffSrc,2*buffW);
        buffDst+=w; buffSrc+=buffW;
    }
}

/*
 * ScreenBuffer and buffer have the same orientation and depth (32 bit)
 * but may differ in size. So upscale/downscale picture accordingly.
 * Scaling should be done here to keep in sync with mouse coordinates.
 * ScreenBuffer rotation is done on java-side, possibly using acceleration
 */
int AND_DoUpdates(short int *ScreenBuffer, int w, int h)
{
    if(AND_device==0)
        return AND_IsRunning;

    if(AND_device->hidden->h > h || AND_device->hidden->w > w)
        AND_DrawDownscale(ScreenBuffer, w, h);
    else if(AND_device->hidden->h*2 <= h && AND_device->hidden->w*2 <= w)
        AND_DrawDoublescale(ScreenBuffer, w, h);
    else
        AND_DrawNoscale(ScreenBuffer, w, h);

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
		SDL_free(this->screen->pixels);
		this->screen->pixels = NULL;
	}
}

JNIEXPORT jint JNICALL Java_libSDL_jni_libSDLproxy_libSDLmain
  (JNIEnv *env, jclass cls, jstring CurrDir)
{
    jboolean iscopy;
    jint ret;
    const char *dir = (*env)->GetStringUTFChars(
                env, CurrDir, &iscopy);
    ret = AND_CallMain(dir);
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
    jint ret;
    if(AND_device==0)
        return -1;

//    jint *Scr = (*env)->GetIntArrayElements(env, arr, 0);
    short int *Scr = (*env)->GetDirectBufferAddress(env, arr);
    ret=AND_DoUpdates(Scr, w, h);
//    (*env)->ReleaseIntArrayElements(env, arr, Scr, 0);
    return ret;
}

JNIEXPORT jint JNICALL Java_libSDL_jni_libSDLproxy_libSDLisportrait
  (JNIEnv *env, jclass jc)
{
    if(AND_device==0)
        return -1;

    return AND_device->hidden->w <= AND_device->hidden->h;
}


JNIEXPORT void JNICALL Java_libSDL_jni_libSDLproxy_libSDLnotifytouch
  (JNIEnv *env, jclass jc, jint flag, jint button, jint x, jint y, jint w, jint h)
{
    if(AND_device==0)
        return;

    if(AND_device->hidden->h > h || AND_device->hidden->w > w)
    {   // downscale
        int sx = (int) (65536.0 * (float) (AND_device->hidden->w - 1) / (float) w);
        int sy = (int) (65536.0 * (float) (AND_device->hidden->h - 1) / (float) h);
        x=(x*sx)/65536;
        y=(y*sy)/65536;
    }     
    else if(AND_device->hidden->h*2 <= h && AND_device->hidden->w*2 <= w)
    {   // double scale
        x/=2; y/=2; w/=2; h/=2;
        int dx=(w-AND_device->hidden->w)/2;
        int dy=(h-AND_device->hidden->h)/2;
        x-=dx; y-=dy;
    }
    else
    {   // no scale
        int dx=(w-AND_device->hidden->w)/2;
        int dy=(h-AND_device->hidden->h)/2;
        x-=dx; y-=dy;
    }
    
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
  (JNIEnv *env, jclass jc, jint flag, jint button, jint x, jint y, jint w, jint h)
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
    if ((*vm)->GetEnv(vm,(void**) &env, JNI_VERSION_1_2) != JNI_OK) {
        AND_puts("Failed to get the environment using GetEnv()");
        return -1;
    }

    jclass cls = (*env)->FindClass(env,"libSDL/jni/libSDLproxy");
    if(!cls) {
        AND_puts("Failed to get libSDLproxy class reference");
        return;
    }
    jmethodID constr = (*env)->GetMethodID(env,cls, "<init>", "()V");
    if(!constr) {
        AND_puts("Failed to get libSDLproxy constructor");
        return;
    }
    jobject obj = (*env)->NewObject(env,cls, constr);
    if(!obj) {
        AND_puts("Failed to create a libSDLproxy object");
        return;
    }
    libSDLproxy = (*env)->NewGlobalRef(env,obj);

    AND_puts("JNI_OnLoad: requesting JNI_VERSION_1_2");

    return JNI_VERSION_1_2;
}

void JNI_OnUnload(JavaVM* vm, void* reserved)
{
}
