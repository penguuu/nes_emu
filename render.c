#include "nes.h"
#include "render.h"
#include "ppu.h"

int init_sdl(){
        int fullscreen = 1;

        if(SDL_Init(SDL_INIT_VIDEO) < 0){
                printf("SDL Initialization failed! SDL_Error: %s\n", SDL_GetError() );
                exit(1);
        }
        else {
                if(fullscreen == 1){
//                        sdl_surface = SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,SDL_SWSURFACE|SDL_DOUBLEBUF);
//
                        sdl_surface = SDL_SetVideoMode(1024,768,SCREEN_BPP,SDL_SWSURFACE|SDL_DOUBLEBUF);
                }
                else {
                        sdl_surface = SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,SDL_SWSURFACE|SDL_DOUBLEBUF);
                }
                SDL_WM_SetCaption("NES EMU","NES EMU");
        }
        if (sdl_surface == NULL){
                printf("Unable to set SDL-surface: %s\n",SDL_GetError());
                exit(1);
        }

        return 1;
}

void close_sdl(){
        SDL_FreeSurface(sdl_surface);
        SDL_Quit();
}

void setpixel(__u16 x, __u16 y, _rgb p){
        __u8 bpp;
        __u32 *s;

        SDL_LockSurface(sdl_surface);
        bpp = sdl_surface->format->BytesPerPixel;

        s = (__u32*)sdl_surface->pixels + (y* sdl_surface->pitch/bpp) + x;
        *s = SDL_MapRGB(sdl_surface->format,p.r,p.g,p.b);

        SDL_UnlockSurface(sdl_surface);
}

void update_screen(_rgb c){
        SDL_Flip(sdl_surface);
        SDL_FillRect(sdl_surface,NULL,SDL_MapRGB(sdl_surface->format,c.r,c.g,c.b));
}
