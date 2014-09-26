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
#include "libSDLproxy.h"
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
	vformat->BitsPerPixel = 32;
	vformat->BytesPerPixel = 4;

	/* We're done! */
	return(0);
}

SDL_Rect **AND_ListModes(_THIS, SDL_PixelFormat *format, Uint32 flags)
{
    if(format->BitsPerPixel!=32)    // only 32 bpp supported
        return 0;
    return (SDL_Rect **) -1;
}

SDL_Surface *AND_SetVideoMode(_THIS, SDL_Surface *current,
				int width, int height, int bpp, Uint32 flags)
{
    if ( bpp!=32 )  // support 32 bit only, other modes would be emulated by SDL
    {
//    	SDL_SetError("only 32 bpp supported");
//        return 0;
        bpp=32;
    }

	if ( this->hidden->buffer ) {
    	SDL_free( this->hidden->buffer );
    	SDL_free( this->hidden->back_buffer );
	}

    this->hidden->back_buffer = SDL_malloc(width * height * (bpp / 8));
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
    char *argv[]={"/sdcard/sdlapp",0,0};

    chdir("/sdcard/SDL/");

    SDL_ShowCursor (SDL_FALSE);

    __android_log_print(ANDROID_LOG_DEBUG,"SDL","calling main()\n");

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

int AND_CallMain()
{
    pthread_attr_t type;
    pthread_attr_init(&type);
    pthread_attr_setdetachstate(&type, PTHREAD_CREATE_JOINABLE);    
    pthread_create (&__threadp, &type, ThreadProc, 0);
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

int AND_DrawDownscale(int *ScreenBuffer, int w, int h)
{
// code is borrowed from SDL_rotozoom.c, interpolated zoom
    static int oldw,oldh;
    static int *sax,*say;
    int x, y, sx, sy, *csax, *csay, csx, csy, ex, ey, t1, t2, sstep;
    tColorRGBA *c00, *c01, *c10, *c11;
    tColorRGBA *sp, *csp, *dp;
    int sgap, dgap;

    /* For interpolation: assume source dimension is one pixel */
    /* smaller to avoid overflow on right and bottom edge.     */
    sx = (int) (65536.0 * (float) (AND_device->hidden->w - 1) / (float) w);
    sy = (int) (65536.0 * (float) (AND_device->hidden->h - 1) / (float) h);

    /* Allocate memory for row increments */
    if(oldw!=w || oldh!=h || sax==0 || say==0)
    {
        if(sax) free(sax);
        if(say) free(say);

        if ((sax = (int *) malloc ((w + 1) * sizeof (Uint32))) == NULL)
        {
          return (-1);
        }
        if ((say = (int *) malloc ((h + 1) * sizeof (Uint32))) == NULL)
        {
          free (sax);
          return (-1);
        }

        /* Precalculate row increments */
        csx = 0;
        csax = sax;
        for (x = 0; x <= w; x++)
        {
          *csax = csx;
          csax++;
          csx &= 0xffff;
          csx += sx;
        }
        csy = 0;
        csay = say;
        for (y = 0; y <= h; y++)
        {
          *csay = csy;
          csay++;
          csy &= 0xffff;
          csy += sy;
        }
        oldw=w; oldh=h;
    }

    /* Pointer setup */
    sp = csp = (tColorRGBA *) AND_device->hidden->back_buffer;
    dp = (tColorRGBA *) ScreenBuffer;
    sgap = 0;
    dgap = 0;

#if 1
  /* Interpolating Zoom */

  /* Scan destination */
    csay = say;
    for (y = 0; y < h; y++)
	{
	  /* Setup color source pointers */
	  c00 = csp;
	  c01 = csp;
	  c01++;
	  c10 = (tColorRGBA *) ((Uint8 *) csp + AND_device->hidden->w*4);
	  c11 = c10;
	  c11++;
	  csax = sax;
	  for (x = 0; x < w; x++)
	    {
	      /* ABGR ordering */
	      /* Interpolate colors */
	      ex = (*csax & 0xffff);
	      ey = (*csay & 0xffff);
	      t1 = ((((c01->r - c00->r) * ex) >> 16) + c00->r) & 0xff;
	      t2 = ((((c11->r - c10->r) * ex) >> 16) + c10->r) & 0xff;
	      dp->r = (((t2 - t1) * ey) >> 16) + t1;
	      t1 = ((((c01->g - c00->g) * ex) >> 16) + c00->g) & 0xff;
	      t2 = ((((c11->g - c10->g) * ex) >> 16) + c10->g) & 0xff;
	      dp->g = (((t2 - t1) * ey) >> 16) + t1;
	      t1 = ((((c01->b - c00->b) * ex) >> 16) + c00->b) & 0xff;
	      t2 = ((((c11->b - c10->b) * ex) >> 16) + c10->b) & 0xff;
	      dp->b = (((t2 - t1) * ey) >> 16) + t1;
	      t1 = ((((c01->a - c00->a) * ex) >> 16) + c00->a) & 0xff;
	      t2 = ((((c11->a - c10->a) * ex) >> 16) + c10->a) & 0xff;
	      dp->a = (((t2 - t1) * ey) >> 16) + t1;
	      /* Advance source pointers */
	      csax++;
	      sstep = (*csax >> 16);
	      c00 += sstep;
	      c01 += sstep;
	      c10 += sstep;
	      c11 += sstep;
	      /* Advance destination pointer */
	      dp++;
	    }
	  /* Advance source pointer */
	  csay++;
	  csp = (tColorRGBA *) ((Uint8 *) csp + (*csay >> 16) * AND_device->hidden->w * 4);
	  /* Advance destination pointers */
	  dp = (tColorRGBA *) ((Uint8 *) dp + dgap);
	}
#else
      /* Non-Interpolating Zoom */

    csay = say;
    for (y = 0; y < h; y++)
	{
	  sp = csp;
	  csax = sax;
	  for (x = 0; x < w; x++)
	    {
	      /* Draw */
	      *dp = *sp;
	      /* Advance source pointers */
	      csax++;
	      sp += (*csax >> 16);
	      /* Advance destination pointer */
	      dp++;
	    }
	  /* Advance source pointer */
	  csay++;
	  csp = (tColorRGBA *) ((Uint8 *) csp + (*csay >> 16) * AND_device->hidden->w*4);
	  /* Advance destination pointers */
	  dp = (tColorRGBA *) ((Uint8 *) dp + dgap);
	}
#endif
    /* Remove temp arrays 
    free (sax);
    free (say); */

    return 0;
}

static void memcpyDbl(int *dst,int *src,int count)
{
    count/=8;   // 8 == 4 bytes in int and size is doubled
    while(count-->0)
    {
        *(dst++)=*src;
        *(dst++)=*(src++);
    }
}

// double pixels, useful for 320x240 modes on 640x480+ devices
int AND_DrawDoublescale(int *ScreenBuffer, int w, int h)
{
    int buffW=AND_device->hidden->w*2, buffH=AND_device->hidden->h*2;
    int *buffSrc=(int*)AND_device->hidden->back_buffer;
    int *buffDst=ScreenBuffer+(w-buffW)/2+((h-buffH)/2)*w;
    memset(ScreenBuffer,0,w*h*4);   // not optimal for speed
    int i;
    for(i=0; i<buffH; i++)
    {
        memcpyDbl(buffDst,buffSrc,4*buffW);
        buffDst+=w; i++;
        memcpyDbl(buffDst,buffSrc,4*buffW);
        buffDst+=w; 
        buffSrc+=buffW/2;
    }
}

// do not scale, add blank lines top/left/rigth/bottom
int AND_DrawNoscale(int *ScreenBuffer, int w, int h)
{
    int buffW=AND_device->hidden->w, buffH=AND_device->hidden->h;
    int *buffSrc=(int*)AND_device->hidden->back_buffer;
    int *buffDst=ScreenBuffer+(w-buffW)/2+((h-buffH)/2)*w;
    memset(ScreenBuffer,0,w*h*4);   // not optimal for speed
    int i;
    for(i=0; i<buffH; i++)
    {
        memcpy(buffDst,buffSrc,4*buffW);
        buffDst+=w; buffSrc+=buffW;
    }
}

/*
 * ScreenBuffer and back_buffer have the same orientation and depth (32 bit)
 * but may differ in size. So upscale/downscale picture accordingly.
 * Scaling should be done here to keep in sync with mouse coordinates.
 * ScreenBuffer rotation is done on java-side, possibly using acceleration
 */
int AND_DoUpdates(int *ScreenBuffer, int w, int h)
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

// code is borrowed from http://android.wooyd.org/JNIExample/
static void AND_UpdateRects(_THIS, int numrects, SDL_Rect *rects)
{
    jclass cls = 0;
    jmethodID midNotify = 0;
    int status;
    JNIEnv *env;
    int isAttached = 0;
   
//    AND_puts("called UpdateRects");

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
        cls=(*env)->GetObjectClass(env,libSDLproxy);
    if(midNotify==0)
        midNotify=(*env)->GetStaticMethodID(env,cls,"javaSDLnotifyneedupdate","()V");

    if(cls==0 || midNotify==0)
    {
        AND_puts("Can't call javaSDLnotifyneedupdate in libSDL.jni.libSDLproxy");
        if(isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
        return;
    }

    AND_device=this;
    memcpy(this->hidden->back_buffer,this->hidden->buffer, this->hidden->h * this->hidden->pitch);

    (*env)->CallStaticObjectMethod(env,cls,midNotify,NULL);

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
        if(this->hidden->back_buffer)
            SDL_free(this->hidden->back_buffer);
		this->screen->pixels = NULL;
	}
}


JNIEXPORT jint JNICALL Java_libSDL_jni_libSDLproxy_libSDLmain
  (JNIEnv *env, jclass jc)
{
     return AND_CallMain();
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
    return;
}

JNIEXPORT void JNICALL Java_libSDL_jni_libSDLproxy_libSDLresume
  (JNIEnv *env, jclass jc)
{
    return;
}

JNIEXPORT jint JNICALL Java_libSDL_jni_libSDLproxy_libSDLupdatebuffer
  (JNIEnv *env, jclass jc, jintArray arr, jint w, jint h)
{
    int ret;
    if(AND_device==0)
        return -1;

    jint *Scr = (*env)->GetIntArrayElements(env, arr, 0);
    ret=AND_DoUpdates(Scr, w, h);
    (*env)->ReleaseIntArrayElements(env, arr, Scr, 0);
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
    if ((*vm)->GetEnv(vm,(void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        AND_puts("Failed to get the environment using GetEnv()");
        return -1;
    }

    jclass cls = (*env)->FindClass(env,"libSDL/jni/libSDLproxy");
    if(!cls) {
        AND_puts("initClassHelper: failed to get libSDLproxy class reference");
        return;
    }
    jmethodID constr = (*env)->GetMethodID(env,cls, "<init>", "()V");
    if(!constr) {
        AND_puts("initClassHelper: failed to get libSDLproxy constructor");
        return;
    }
    jobject obj = (*env)->NewObject(env,cls, constr);
    if(!obj) {
        AND_puts("initClassHelper: failed to create a libSDLproxy object");
        return;
    }
    libSDLproxy = (*env)->NewGlobalRef(env,obj);

    AND_puts("JNI_OnLoad: returning JNI_VERSION_1_4");

    return JNI_VERSION_1_4;
}

void JNI_OnUnload(JavaVM* vm, void* reserved)
{
}
