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

    This file written by Ryan C. Gordon (icculus@icculus.org)
*/
#include "SDL_config.h"

/* Output audio to nowhere... */

#include "SDL_rwops.h"
#include "SDL_timer.h"
#include "SDL_audio.h"
#include "../SDL_audiomem.h"
#include "../SDL_audio_c.h"
#include "../SDL_audiodev_c.h"
#include "SDL_andaudio.h"
#include "/home/nathan/android-gcc-450/arm-linux-elf/include/jni.h"

/* The tag name used by DUMMY audio */
#define ANDAUD_DRIVER_NAME         "android"

/* Audio driver functions */
static int ANDAUD_OpenAudio(_THIS, SDL_AudioSpec *spec);
static void ANDAUD_WaitAudio(_THIS);
static void ANDAUD_PlayAudio(_THIS);
static Uint8 *ANDAUD_GetAudioBuf(_THIS);
static void ANDAUD_CloseAudio(_THIS);
extern jobject libSDLproxy;       // Java part inplementation ptr
extern JavaVM *gJavaVM;

/* Audio driver bootstrap functions */
static int ANDAUD_Available(void)
{
	const char *envr = SDL_getenv("SDL_AUDIODRIVER");
	if (envr && (SDL_strcmp(envr, ANDAUD_DRIVER_NAME) == 0)) {
		return(1);
	}
	return(1);
}

static void ANDAUD_DeleteDevice(SDL_AudioDevice *device)
{
	SDL_free(device->hidden);
	SDL_free(device);
}

static SDL_AudioDevice *ANDAUD_CreateDevice(int devindex)
{
	SDL_AudioDevice *this;

	/* Initialize all variables that we clean on shutdown */
	this = (SDL_AudioDevice *)SDL_malloc(sizeof(SDL_AudioDevice));
	if ( this ) {
		SDL_memset(this, 0, (sizeof *this));
		this->hidden = (struct SDL_PrivateAudioData *)
				SDL_malloc((sizeof *this->hidden));
	}
	if ( (this == NULL) || (this->hidden == NULL) ) {
		SDL_OutOfMemory();
		if ( this ) {
			SDL_free(this);
		}
		return(0);
	}
	SDL_memset(this->hidden, 0, (sizeof *this->hidden));

	/* Set the function pointers */
	this->OpenAudio = ANDAUD_OpenAudio;
	this->WaitAudio = ANDAUD_WaitAudio;
	this->PlayAudio = ANDAUD_PlayAudio;
	this->GetAudioBuf = ANDAUD_GetAudioBuf;
	this->CloseAudio = ANDAUD_CloseAudio;

	this->free = ANDAUD_DeleteDevice;

	return this;
}

struct AudioBootStrap ANDAUD_bootstrap = {
	ANDAUD_DRIVER_NAME, "SDL android audio driver",
	ANDAUD_Available, ANDAUD_CreateDevice
};

/* This function waits until it is possible to write a full sound buffer */
static void ANDAUD_WaitAudio(_THIS)
{
//	SDL_Delay(this->hidden->write_delay);
}

static int ANDAUD_GetMinSize(int freq, int channels)
{
    jclass cls = 0;
    jmethodID midNotify = 0;
    int status;
    JNIEnv *env;
    int isAttached = 0;
    int ret=0;
   
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
        cls=(*env)->GetObjectClass(env,libSDLproxy);   
        if(cls==0)
        {
            AND_puts("Can't find class libSDL.jni.libSDLproxy");
            if(isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
            return 0;
        }
    }

    if(midNotify==0)
    {
        midNotify=(*env)->GetStaticMethodID(env,cls,"javaSDLinitaudio","(II)I");
        if(midNotify==0)
        {
            AND_puts("Can't find javaSDLinitaudio in libSDL.jni.libSDLproxy");
            if(isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
            return 0;
        }
    }

    ret = (*env)->CallStaticIntMethod(env,cls,midNotify,(jint)freq,(jint)channels,NULL);

    if(isAttached) 
        (*gJavaVM)->DetachCurrentThread(gJavaVM);

    return ret;
}

static void ANDAUD_PlayAudio(_THIS)
{
    jclass cls = 0;
    jmethodID midNotify = 0;
    int status;
    JNIEnv *env;
    int isAttached = 0;
    int ret=0;
    jshortArray Buffer=0;
   
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
        cls=(*env)->GetObjectClass(env,libSDLproxy);   
        if(cls==0)
        {
            AND_puts("Can't find class libSDL.jni.libSDLproxy");
            if(isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
            return;
        }
    }

    if(midNotify==0)
    {
        midNotify=(*env)->GetStaticMethodID(env,cls,"javaSDLwriteaudio","([SI)V");
        if(midNotify==0)
        {
            AND_puts("Can't find javaSDLwriteaudio in libSDL.jni.libSDLproxy");
            if(isAttached) (*gJavaVM)->DetachCurrentThread(gJavaVM);
            return;
        }
    }

    Buffer = (*env)->NewShortArray(env, this->hidden->mixlen/2);
    (*env)->SetShortArrayRegion(env, Buffer,
                        0, this->hidden->mixlen/2, (const jshort*)this->hidden->mixbuf);

    /* Write the audio data */
    (*env)->CallStaticVoidMethod(env,cls,midNotify,Buffer,(jint)this->hidden->mixlen,NULL);

    if(isAttached) 
        (*gJavaVM)->DetachCurrentThread(gJavaVM);
}

static Uint8 *ANDAUD_GetAudioBuf(_THIS)
{
	return(this->hidden->mixbuf);
}

static void ANDAUD_CloseAudio(_THIS)
{
	if ( this->hidden->mixbuf != NULL ) {
		SDL_FreeAudioMem(this->hidden->mixbuf);
		this->hidden->mixbuf = NULL;
	}
}

static int ANDAUD_OpenAudio(_THIS, SDL_AudioSpec *spec)
{
	float bytes_per_sec = 0.0f;

    spec->format = AUDIO_S16;  // only signed 16 bit stereo is supported
    if(spec->channels>2 || spec->channels<1)
        spec->channels=2;

	/* Allocate mixing buffer */
	this->hidden->mixlen = ANDAUD_GetMinSize(spec->freq,spec->channels);
    bytes_per_sec = (float) (((spec->format & 0xFF) / 8) *
	                   spec->channels * spec->freq);

    if(this->hidden->mixlen<spec->size)
        this->hidden->mixlen=spec->size;

    spec->size=this->hidden->mixlen;

//    AND_printf("Audio bufer len: %d\n",this->hidden->mixlen);

	this->hidden->mixbuf = (Uint8 *) SDL_AllocAudioMem(this->hidden->mixlen);
	if ( this->hidden->mixbuf == NULL ) {
		return(-1);
	}
	SDL_memset(this->hidden->mixbuf, spec->silence, spec->size);

	/*
	 * We try to make this request more audio at the correct rate for
	 *  a given audio spec, so timing stays fairly faithful.
	 * Also, we have it not block at all for the first two calls, so
	 *  it seems like we're filling two audio fragments right out of the
	 *  gate, like other SDL drivers tend to do.
	 */
	this->hidden->initial_calls = 2;
	this->hidden->write_delay =
	               (Uint32) ((((float) spec->size) / bytes_per_sec) * 1000.0f);

	/* We're ready to rock and roll. :-) */
	return(0);
}

