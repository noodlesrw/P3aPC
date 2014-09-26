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

/* Being a null driver, there's no event stream. We just define stubs for
   most of the API. */

#include "SDL.h"
#include "../../events/SDL_sysevents.h"
#include "../../events/SDL_events_c.h"
#include "SDL_joystick.h"
#include "../../joystick/SDL_sysjoystick.h"
#include "../../joystick/SDL_joystick_c.h"

#include <jni.h>

#include "SDL_andvideo.h"
#include "SDL_andevents_c.h"

void AND_InitOSKeymap(_THIS)
{
	/* do nothing. */
}

static int scancodes[SDLK_LAST];
static int Android2SDL[100];
void InitScancodes()
{
    if(scancodes[SDLK_ESCAPE])
        return;

    Android2SDL[0]=SDLK_UNKNOWN;
    Android2SDL[1]=SDLK_UNKNOWN;
    Android2SDL[2]=SDLK_UNKNOWN;
    Android2SDL[3]=SDLK_HOME;
    Android2SDL[4]=SDLK_ESCAPE; // BACK
    Android2SDL[5]=SDLK_UNKNOWN;
    Android2SDL[6]=SDLK_UNKNOWN;
    Android2SDL[7]=SDLK_0;
    Android2SDL[8]=SDLK_1;
    Android2SDL[9]=SDLK_2;
    Android2SDL[10]=SDLK_3;
    Android2SDL[11]=SDLK_4;
    Android2SDL[12]=SDLK_5;
    Android2SDL[13]=SDLK_6;
    Android2SDL[14]=SDLK_7;
    Android2SDL[15]=SDLK_8;
    Android2SDL[16]=SDLK_9;
    Android2SDL[17]=SDLK_ASTERISK;
    Android2SDL[18]=SDLK_UNKNOWN;
    Android2SDL[19]=SDLK_UP;
    Android2SDL[20]=SDLK_DOWN;
    Android2SDL[21]=SDLK_LEFT;
    Android2SDL[22]=SDLK_RIGHT;
    Android2SDL[23]=SDLK_RETURN;
    Android2SDL[24]=SDLK_KP_PLUS;
    Android2SDL[25]=SDLK_KP_MINUS;
    Android2SDL[26]=SDLK_POWER;
    Android2SDL[27]=SDLK_KP_ENTER;
    Android2SDL[28]=SDLK_CLEAR;
    Android2SDL[29]=SDLK_a;
    Android2SDL[30]=SDLK_b;
    Android2SDL[31]=SDLK_c;
    Android2SDL[32]=SDLK_d;
    Android2SDL[33]=SDLK_e;
    Android2SDL[34]=SDLK_f;
    Android2SDL[35]=SDLK_g;
    Android2SDL[36]=SDLK_h;
    Android2SDL[37]=SDLK_i;
    Android2SDL[38]=SDLK_j;
    Android2SDL[39]=SDLK_k;
    Android2SDL[40]=SDLK_l;
    Android2SDL[41]=SDLK_m;
    Android2SDL[42]=SDLK_n;
    Android2SDL[43]=SDLK_o;
    Android2SDL[44]=SDLK_p;
    Android2SDL[45]=SDLK_q;
    Android2SDL[46]=SDLK_r;
    Android2SDL[47]=SDLK_s;
    Android2SDL[48]=SDLK_t;
    Android2SDL[49]=SDLK_u;
    Android2SDL[50]=SDLK_v;
    Android2SDL[51]=SDLK_w;
    Android2SDL[52]=SDLK_x;
    Android2SDL[53]=SDLK_y;
    Android2SDL[54]=SDLK_z;
    Android2SDL[55]=SDLK_COMMA;
    Android2SDL[56]=SDLK_PERIOD;
    Android2SDL[57]=SDLK_LALT;
    Android2SDL[58]=SDLK_RALT;
    Android2SDL[59]=SDLK_LSHIFT;
    Android2SDL[60]=SDLK_RSHIFT;
    Android2SDL[61]=SDLK_TAB;
    Android2SDL[62]=SDLK_SPACE;
    Android2SDL[63]=SDLK_LSUPER;
    Android2SDL[64]=SDLK_RSUPER;
    Android2SDL[65]=SDLK_MODE;
    Android2SDL[66]=SDLK_RETURN;
    Android2SDL[67]=SDLK_DELETE;
    Android2SDL[68]=SDLK_UNKNOWN;
    Android2SDL[69]=SDLK_MINUS;
    Android2SDL[70]=SDLK_EQUALS;
    Android2SDL[71]=SDLK_LEFTBRACKET;
    Android2SDL[72]=SDLK_RIGHTBRACKET;
    Android2SDL[73]=SDLK_BACKSLASH;
    Android2SDL[74]=SDLK_SEMICOLON;
    Android2SDL[75]=SDLK_QUOTE;
    Android2SDL[76]=SDLK_SLASH;
    Android2SDL[77]=SDLK_AT;
    Android2SDL[78]=SDLK_NUMLOCK;
    Android2SDL[79]=SDLK_UNKNOWN;
    Android2SDL[80]=SDLK_UNKNOWN;
    Android2SDL[81]=SDLK_PLUS;
    Android2SDL[82]=SDLK_LCTRL;//SDLK_MENU;
    Android2SDL[83]=SDLK_UNKNOWN;
    Android2SDL[84]=SDLK_SPACE;//SDLK_SEARCH;
    Android2SDL[85]=SDLK_F1;//SDLK_MEDIA_PLAY_PAUSE;
    Android2SDL[86]=SDLK_F2;//SDLK_MEDIA_STOP;
    Android2SDL[87]=SDLK_F3;//SDLK_MEDIA_NEXT;
    Android2SDL[88]=SDLK_F4;//SDLK_MEDIA_PREVIOUS;
    Android2SDL[89]=SDLK_F5;//SDLK_MEDIA_REWIND;
    Android2SDL[90]=SDLK_F6;//SDLK_MEDIA_FAST_FORWARD;
    Android2SDL[91]=SDLK_F7;//SDLK_MUTE;
    Android2SDL[92]=SDLK_PAGEUP;
    Android2SDL[93]=SDLK_PAGEDOWN;


    scancodes[SDLK_ESCAPE]=0x01;
    scancodes[SDLK_F1]=0x3b;
    scancodes[SDLK_F2]=0x3c;
    scancodes[SDLK_F3]=0x3d;
    scancodes[SDLK_F4]=0x3e;
    scancodes[SDLK_F5]=0x3f;
    scancodes[SDLK_F6]=0x40;
    scancodes[SDLK_F7]=0x41;
    scancodes[SDLK_F8]=0x42;
    scancodes[SDLK_F9]=0x43;
    scancodes[SDLK_F10]=0x44;
    scancodes[SDLK_F11]=0x57;
    scancodes[SDLK_F12]=0x58;
    scancodes[SDLK_HOME]=0x47;
    scancodes[SDLK_END]=0x4f;
    scancodes[SDLK_PAGEUP]=0x49;
    scancodes[SDLK_BREAK]=0x77;
    scancodes[SDLK_SCROLLOCK]=0x46;
    scancodes[SDLK_KP_MULTIPLY]=0x37;
    scancodes[SDLK_KP_MINUS]=0x4a;
    scancodes[SDLK_KP_PLUS]=0x4e;
    scancodes[SDLK_MENU]=0x5d;
    scancodes[SDLK_BACKQUOTE]=0x29;
    scancodes[SDLK_1]=0x2;
    scancodes[SDLK_2]=0x3;
    scancodes[SDLK_3]=0x4;
    scancodes[SDLK_4]=0x5;
    scancodes[SDLK_5]=0x6;
    scancodes[SDLK_6]=0x7;
    scancodes[SDLK_7]=0x8;
    scancodes[SDLK_8]=0x9;
    scancodes[SDLK_9]=0xa;
    scancodes[SDLK_0]=0xb;
    scancodes[SDLK_MINUS]=0xc;
    scancodes[SDLK_EQUALS]=0xd;
    scancodes[SDLK_BACKSPACE]=0xe;
    scancodes[SDLK_PAGEDOWN]=0x51;
    scancodes[SDLK_PRINT]=0x55;
    scancodes[SDLK_NUMLOCK]=0x45;
    scancodes[SDLK_KP7]=0x47;
    scancodes[SDLK_KP8]=0x48;
    scancodes[SDLK_KP9]=0x49;
    scancodes[SDLK_LSUPER]=0x38;
    scancodes[SDLK_TAB]=0x0f;
    scancodes[SDLK_q]=0x10;
    scancodes[SDLK_w]=0x11;
    scancodes[SDLK_e]=0x12;
    scancodes[SDLK_r]=0x13;
    scancodes[SDLK_t]=0x14;
    scancodes[SDLK_y]=0x15;
    scancodes[SDLK_u]=0x16;
    scancodes[SDLK_i]=0x17;
    scancodes[SDLK_o]=0x18;
    scancodes[SDLK_p]=0x19;
    scancodes[SDLK_LEFTBRACKET]=0x1a;
    scancodes[SDLK_RIGHTBRACKET]=0x1b;
    scancodes[SDLK_BACKSLASH]=0x2b;
    scancodes[SDLK_SYSREQ]=0x54;
    scancodes[SDLK_RSUPER]=0x38;        // window
    scancodes[SDLK_KP_DIVIDE]=0x35;
    scancodes[SDLK_KP4]=0x4b;
    scancodes[SDLK_KP5]=0x4c;
    scancodes[SDLK_KP6]=0x4d;
    scancodes[SDLK_CAPSLOCK]=0x3a;
    scancodes[SDLK_LALT]=0x38;
    scancodes[SDLK_a]=0x1e;
    scancodes[SDLK_s]=0x1f;
    scancodes[SDLK_d]=0x20;
    scancodes[SDLK_f]=0x21;
    scancodes[SDLK_g]=0x22;
    scancodes[SDLK_h]=0x23;
    scancodes[SDLK_j]=0x24;
    scancodes[SDLK_k]=0x25;
    scancodes[SDLK_l]=0x26;
    scancodes[SDLK_SEMICOLON]=0x27;
    scancodes[SDLK_QUOTE]=0x28;
    scancodes[SDLK_RALT]=0x38;
    scancodes[SDLK_INSERT]=0x52;
    scancodes[SDLK_RCTRL]=0x1d;
    scancodes[SDLK_UP]=0x48;
    scancodes[SDLK_RSHIFT]=0x36;
    scancodes[SDLK_KP1]=0x4f;
    scancodes[SDLK_KP2]=0x50;
    scancodes[SDLK_KP3]=0x51;
    scancodes[SDLK_LCTRL]=0x1d;
    scancodes[SDLK_LSHIFT]=0x2a;
    scancodes[SDLK_z]=0x2c;
    scancodes[SDLK_x]=0x2d;
    scancodes[SDLK_c]=0x2e;
    scancodes[SDLK_v]=0x2f;
    scancodes[SDLK_b]=0x30;
    scancodes[SDLK_SPACE]=0x39;
    scancodes[SDLK_n]=0x31;
    scancodes[SDLK_m]=0x32;
    scancodes[SDLK_COMMA]=0x33;
    scancodes[SDLK_PERIOD]=0x34;
    scancodes[SDLK_SLASH]=0x35;
    scancodes[SDLK_RETURN]=0x1c;
    scancodes[SDLK_DELETE]=0x53;
    scancodes[SDLK_LEFT]=0x4b;
    scancodes[SDLK_DOWN]=0x50;
    scancodes[SDLK_RIGHT]=0x4d;
    scancodes[SDLK_KP0]=0x52;
    scancodes[SDLK_KP_PERIOD]=0x53;
    scancodes[SDLK_KP_ENTER]=0x1c;
}

JNIEXPORT void JNICALL Java_libSDL_jni_libSDLproxy_libSDLnotifykey
  (JNIEnv *env, jclass jc, jint flag, jint key)
{
    static SDL_keysym Key;
    InitScancodes();

    Key.sym=Android2SDL[key]; 
    Key.scancode=scancodes[Key.sym%SDLK_LAST]+8;    // linux adds 8, Dosbox expects this
    Key.mod=0;
    Key.unicode=0;
    if(Key.sym)
    {
        SDL_PrivateKeyboard(flag, &Key);
    }
}

extern SDL_Joystick **SDL_joysticks;
JNIEXPORT void JNICALL Java_libSDL_jni_libSDLproxy_libSDLnotifyjoystick
  (JNIEnv *env, jclass jc, jint X, jint Y, jint butt)
{
    if(SDL_joysticks)
    {
        SDL_PrivateJoystickAxis(SDL_joysticks[0], 0, X);
        SDL_PrivateJoystickAxis(SDL_joysticks[0], 1, Y);
        if(butt>=0)
            SDL_PrivateJoystickButton(SDL_joysticks[0], 0, butt);
    }
}
/*
extern SDL_Joystick **SDL_joysticks;
JNIEXPORT void JNICALL Java_libSDL_jni_libSDLproxy_libSDLnotifyjoystick
  (JNIEnv *env, jclass jc, jint pBall, jint pX, jint pY, jint pAxis, jint pOff, jint pButton, jint pState)
{
    if(SDL_joysticks)
    {
		if ( pAxis >= 0 )
			SDL_PrivateJoystickAxis(SDL_joysticks[0], pAxis, pOff);
		if ( pBall >= 0 )
			SDL_PrivateJoystickBall(SDL_joysticks[0], pBall, pX, pY);
        if(pButton>=0)
            SDL_PrivateJoystickButton(SDL_joysticks[0], pButton, pState);
    }
}
*/
/*
extern int SDL_PrivateJoystickAxis(SDL_Joystick *joystick,
                                   Uint8 axis, Sint16 value);
extern int SDL_PrivateJoystickBall(SDL_Joystick *joystick,
                                   Uint8 ball, Sint16 xrel, Sint16 yrel);
extern int SDL_PrivateJoystickHat(SDL_Joystick *joystick,
                                 Uint8 hat, Uint8 value);
extern int SDL_PrivateJoystickButton(SDL_Joystick *joystick,
                                     Uint8 button, Uint8 state);
*/
