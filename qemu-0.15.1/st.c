#include <SDL.h>
#undef main /* We don't want SDL to override our main() */
int main( void ) { return SDL_Init (SDL_INIT_VIDEO); }
