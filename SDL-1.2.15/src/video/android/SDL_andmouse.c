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

#include <sys/ioctl.h>

#include "SDL_mouse.h"
#include "../../events/SDL_events_c.h"
#include "../SDL_cursor_c.h"
#include "SDL_andvideo.h"
#include "SDL_andmouse_c.h"

/* The implementation dependent data for the window manager cursor */
struct WMcursor {
	int unused;
};

/* There isn't any implementation dependent data */
void AND_FreeWMCursor(_THIS, WMcursor *cursor)
{
	return;
}

/* There isn't any implementation dependent data */
WMcursor *AND_CreateWMCursor(_THIS,
		Uint8 *data, Uint8 *mask, int w, int h, int hot_x, int hot_y)
{
	return ((WMcursor *)0x01);
}

static void AND_MoveCursor(_THIS, SDL_Cursor *cursor, int x, int y)
{
	/* Lock so we don't interrupt an update with mouse motion */
	SDL_LockCursor();

//	SDL_EraseCursorNoLock(this->screen);
//	SDL_DrawCursorNoLock(this->screen);

	/* We're finished */
	SDL_UnlockCursor();
}

void AND_MoveWMCursor(_THIS, int x, int y)
{
	AND_MoveCursor(this, SDL_cursor, x, y);
}

int AND_ShowWMCursor(_THIS, WMcursor *wmcursor)
{
	SDL_Cursor *cursor;
	int x, y;

	/* Draw the cursor at the appropriate location */
    SDL_GetMouseState(&x, &y);
    SDL_cursor->area.x = (x - SDL_cursor->hot_x);
    SDL_cursor->area.y = (y - SDL_cursor->hot_y);	
    if ( wmcursor ) {
		cursor = SDL_cursor;
	} else {
		cursor = NULL;
	}
	AND_MoveCursor(this, cursor, x, y);
	return(1);
}
