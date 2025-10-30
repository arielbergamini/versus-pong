
#include <SDL2/SDL.h>
#include <iostream>


int main (int argc, char* argv[]) {
    
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to initialize SDL2 library\n");
        return -2;
    }

    SDL_Window *window = SDL_CreateWindow("Versus Pong!",
                                            SDL_WINDOWPOS_CENTERED,
                                            SDL_WINDOWPOS_CENTERED,
                                            680, 480, 0);
    if (!window) {
        printf("Failed to create window\n");
        return -1;
    }
    
    SDL_Surface *window_surface = SDL_GetWindowSurface(window);
    
    if (!window_surface) {
        printf("Failed to get surface from window\n");
        return -1;
    }

    SDL_UpdateWindowSurface(window);

    SDL_Delay(5000);
    SDL_DestroyWindow(window);
    
    
    return 0;
}