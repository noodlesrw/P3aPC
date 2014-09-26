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

/* Android SDL video driver implementation */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#ifndef HAVE_GETPAGESIZE
#include <asm/page.h>		/* For definition of PAGE_SIZE */
#endif

#include <linux/vt.h>
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
static pthread_t __threadp=0;           // thread where main() is running
static int AND_IsRunning=1;             // False if app asks to terminate
static int console_fd=-1;               // /dev/graphics/fb0 handle

static char* mapped_mem=0;              // Vmem info, all code borrowed from FB_con
static struct fb_fix_screeninfo finfo;  // display information, kept here as we never change mode
static struct fb_var_screeninfo vinfo;
static int AND_UseDirectFB=0;           // ==1 if we've returned direct pointer to videomemory


/* Small wrapper for mmap() so we can play nicely with no-mmu hosts
 * (non-mmu hosts disallow the MAP_SHARED flag) */

static void *do_mmap(void *start, size_t length, int prot, int flags, int fd, off_t offset)
{
	void *ret;
	ret = mmap(start, length, prot, flags, fd, offset);
	if ( ret == (char *)-1 && (flags & MAP_SHARED) ) {
		ret = mmap(start, length, prot,
		           (flags & ~MAP_SHARED) | MAP_PRIVATE, fd, offset);
	}
	return ret;
}

/* AND driver bootstrap functions */

static int AND_Available(void)
{
	const char *envr = SDL_getenv("SDL_VIDEODRIVER");
    int console = -1;
	if ((envr) && (SDL_strcmp(envr, ANDVID_DRIVER_NAME) == 0)) {
		return(1);
	}

   	console = open("/dev/graphics/fb0", O_RDWR, 0);
	if ( console >= 0 )
		close(console);
	
    return(console >= 0);
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
	device->InitOSKeymap = AND_InitOSKeymap;
	device->PumpEvents = AND_PumpEvents;

	device->free = AND_DeleteDevice;

	return device;
}

VideoBootStrap AND_bootstrap = {
	ANDVID_DRIVER_NAME, "SDL Android direct fb0 video driver",
	AND_Available, AND_CreateDevice
};


int AND_VideoInit(_THIS, SDL_PixelFormat *vformat)
{
    const int pagesize = SDL_getpagesize();
	const char *SDL_fbdev;

    SDL_fbdev = SDL_getenv("SDL_FBDEV");
	if ( SDL_fbdev == NULL ) 
		SDL_fbdev = "/dev/graphics/fb0";

    console_fd = open(SDL_fbdev, O_RDWR, 0);
	if ( console_fd < 0 ) {
		SDL_SetError("Unable to open %s", SDL_fbdev);
		return(-1);
	}

    /* Get the type of video hardware */
	if ( ioctl(console_fd, FBIOGET_FSCREENINFO, &finfo) < 0 ) {
		SDL_SetError("Couldn't get console hardware info");
		return(-1);
	}
	switch (finfo.type) {
		case FB_TYPE_PACKED_PIXELS:
			/* Supported, no worries.. */
			break;
		default:
			SDL_SetError("Unsupported console hardware");
			return(-1);
	}
	switch (finfo.visual) {
		case FB_VISUAL_TRUECOLOR:
		case FB_VISUAL_PSEUDOCOLOR:
		case FB_VISUAL_STATIC_PSEUDOCOLOR:
		case FB_VISUAL_DIRECTCOLOR:
			break;
		default:
			SDL_SetError("Unsupported console hardware");
			return(-1);
	}

	mapped_mem = do_mmap(NULL, finfo.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, console_fd, 0);
	if ( mapped_mem == (char *)-1 ) {
		SDL_SetError("Unable to memory map the video hardware");
		mapped_mem = NULL;
		FB_VideoQuit(this);
		return(-1);
	}

    /* Determine the current screen depth */
	if ( ioctl(console_fd, FBIOGET_VSCREENINFO, &vinfo) < 0 ) {
		SDL_SetError("Couldn't get console pixel format");
		return(-1);
	}
	vformat->BitsPerPixel = vinfo.bits_per_pixel;
	if ( vformat->BitsPerPixel < 8 ) {
    	SDL_SetError("Less then 256 colors are not supported");
		return(-1);
	}
	for ( i=0; i<vinfo.red.length; ++i ) {
		vformat->Rmask <<= 1;
		vformat->Rmask |= (0x00000001<<vinfo.red.offset);
	}
	for ( i=0; i<vinfo.green.length; ++i ) {
		vformat->Gmask <<= 1;
		vformat->Gmask |= (0x00000001<<vinfo.green.offset);
	}
	for ( i=0; i<vinfo.blue.length; ++i ) {
		vformat->Bmask <<= 1;
		vformat->Bmask |= (0x00000001<<vinfo.blue.offset);
	}

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
    int req_orientation=width<height;
    int our_orientation=vinfo.xres<vinfo.yres;

    if ( mapped_mem==0 )  
    {
    	SDL_SetError("Video memory is not mapped!");
        return 0;
    }

    bpp=vinfo.bits_per_pixel;   // let SDL emulate other bit depths for us

    if(AND_UseDirectFB==0 && this->hidden->buffer )
    	SDL_free( this->hidden->buffer );

    // Determine - should we return direct ptr to vmem or not
    AND_UseDirectFB = req_orientation==our_orientation;

    if(width>vinfo.xres || height>vinfo.yres)
        AND_UseDirectFB=0;
    if(! (width*2<vinfo.xres && height*2<vinfo.yres))   // would use optimized pixel-doubled mode
        AND_UseDirectFB=0;

    if((flags&SDL_HWSURFACE) == 0)
        AND_UseDirectFB=0;

    if(AND_UseDirectFB)
        this->hidden->buffer = mapped_mem;
    else
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

    if(AND_UseDirectFB)
        current->pitch = finfo.line_length;
    else
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
static int NeedUpdates=1;
int AND_DoUpdates(int *ScreenBuffer, int w, int h)
{
    if(AND_device==0 || !NeedUpdates)
        return AND_IsRunning;

    if(AND_device->hidden->h > h || AND_device->hidden->w > w)
        AND_DrawDownscale(ScreenBuffer, w, h);
    else if(AND_device->hidden->h*2 <= h && AND_device->hidden->w*2 <= w)
        AND_DrawDoublescale(ScreenBuffer, w, h);
    else
        AND_DrawNoscale(ScreenBuffer, w, h);

//    NeedUpdates = 0;
    return AND_IsRunning;
}

static void AND_UpdateRects(_THIS, int numrects, SDL_Rect *rects)
{
    AND_device=this;
    memcpy(this->hidden->back_buffer,this->hidden->buffer, this->hidden->h * this->hidden->pitch);
    NeedUpdates = 1;
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
  (JNIEnv *env, jclass jc)
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
    jint *Scr = (*env)->GetIntArrayElements(env, arr, 0);
    AND_DoUpdates(Scr, w, h);
    (*env)->ReleaseIntArrayElements(env, arr, Scr, 0);
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

